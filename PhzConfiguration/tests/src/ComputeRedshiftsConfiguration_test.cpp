/*
 * ComputeRedshiftsConfiguration_test.cpp
 *
 *  Created on: May 28, 2015
 *      Author: Nicolas Morisset
 */


#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "PhzConfiguration/ComputeRedshiftsConfiguration.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzUtils/FileUtils.h"

using namespace Euclid;
namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;
namespace fs = boost::filesystem;

struct ComputeRedshiftsConfiguration_Fixture {

  const std::string CATALOG_TYPE {"catalog-type"
      ""};
  const std::string RESULTS_DIR {"results-dir"};
  const std::string AXES_COLLAPSE_TYPE {"axes-collapse-type"};
  const std::string OUTPUT_CATALOG_FORMAT {"output-catalog-format"};
  const std::string PHZ_OUTPUT_DIR {"phz-output-dir"};
  const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
  const std::string INPUT_CATALOG_FORMAT {"input-catalog-format"};
  const std::string CREATE_OUTPUT_CATALOG_FLAG {"create-output-catalog"};
  const std::string CREATE_OUTPUT_PDF_FLAG {"create-output-pdf"};
  const std::string CREATE_OUTPUT_POSTERIORS_FLAG {"create-output-posteriors"};
  const std::string MODEL_GRID_FILE {"model-grid-file"};
  const std::string PHOTOMETRIC_CORRECTION_FILE {"photometric-correction-file"};
  const std::string ENABLE_PHOTOMETRIC_CORRECTION {"enable-photometric-correction"};
  const std::string FILTER_MAPPING_FILE {"filter-mapping-file"};

  Elements::TempDir temp_dir {};
  fs::path results_dir = temp_dir.path();
  fs::path input_catalog = temp_dir.path() / "input_catalog.txt";
  fs::path model_grid = temp_dir.path() / "model_grid.txt";
  fs::path filter_mapping = temp_dir.path() / "filter_mapping.txt";
  fs::path phz_out_dir = temp_dir.path() / "out_dir";
  fs::path phot_corr = temp_dir.path() / "phot_corr.txt";

  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{(fs::path("reddeningCurves") / fs::path("Curve1")).string()}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{(fs::path("sed") / fs::path("Curve1")).string()}};

  std::map<std::string, po::variable_value> options_map;

  SourceCatalog::Source source {123, {}};
  PhzDataModel::PhotometryGrid res_phot_grid {PhzDataModel::createAxesTuple(
          {0.}, {0.}, {{"Name"}}, {{"Name"}}
  )};
  PhzDataModel::Pdf1D res_pdf {{"Z", {0.}}};
  PhzDataModel::LikelihoodGrid res_likelihood {res_phot_grid.getAxesTuple()};
  PhzDataModel::LikelihoodGrid res_posterior {res_phot_grid.getAxesTuple()};

  std::map<std::string, PhzDataModel::LikelihoodGrid> makePosteriorMap(PhzDataModel::LikelihoodGrid&& posterior) {
    std::map<std::string, PhzDataModel::LikelihoodGrid> result {};
    result.emplace(std::string(""), std::move(posterior));
    return result;
  }
  std::map<std::string, PhzDataModel::LikelihoodGrid> likelihood_map = makePosteriorMap(std::move(res_likelihood));
  std::map<std::string, PhzDataModel::LikelihoodGrid> posterior_map = makePosteriorMap(std::move(res_posterior));
  PhzOutput::OutputHandler::result_type res {
        res_phot_grid.cbegin(),
        std::move(res_pdf),
        std::move(likelihood_map),
        std::move(posterior_map),
        0.,
        0.,
        std::map<std::string, double>{{"", 0.}}
  };



  ComputeRedshiftsConfiguration_Fixture() {
    std::ofstream cat_out {input_catalog.string()};
    cat_out << "# ID      Z        Z_ERR    F1      F1_ERR  F2      F2_ERR\n"
            << "# long    double   double   double  double  double  double\n"
            << "\n"
            << "1         0.25     0.01     1.      0.1     3.      0.3\n"
            << "2         1.01     0.02     2.      0.2     4.      0.4\n";
    cat_out.close();
    options_map[INPUT_CATALOG_FILE].value() = boost::any(input_catalog.string());

    std::ofstream map_out {filter_mapping.string()};
    map_out << "Filter1 F1 F1_ERR\n"
            << "Filter2 F2 F2_ERR\n";
    map_out.close();
    options_map[FILTER_MAPPING_FILE].value() = boost::any(filter_mapping.string());

    // Create files
    std::ofstream correction_file(phot_corr.string());
    // Fill up file
    correction_file << "#Filter Correction\n";
    correction_file << "#string     double\n";
    correction_file << "Filter1     1.1 \n";
    correction_file << "Filter2     2.2 \n";
    correction_file.close();
    options_map[PHOTOMETRIC_CORRECTION_FILE].value() = boost::any(phot_corr.string());
    options_map[ENABLE_PHOTOMETRIC_CORRECTION].value() = boost::any(std::string{"YES"});

    PhzDataModel::PhotometryGridInfo grid_info;
    grid_info.filter_names = {{"Filter1"}, {"Filter2"}};
    std::ofstream grid_out {model_grid.string()};
    boost::archive::binary_oarchive boa {grid_out};
    boa << grid_info;
    grid_out.close();
    options_map[MODEL_GRID_FILE].value() = boost::any(model_grid.string());

    options_map[PHZ_OUTPUT_DIR].value() = boost::any(phz_out_dir.string());
    options_map[CATALOG_TYPE].value() = boost::any(std::string{"CatalogType"});
    options_map[RESULTS_DIR].value() = boost::any(results_dir.string());
  }
  ~ComputeRedshiftsConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputeRedshiftsConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::ComputeRedshiftsConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow(AXES_COLLAPSE_TYPE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(OUTPUT_CATALOG_FORMAT, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(PHZ_OUTPUT_DIR, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(INPUT_CATALOG_FILE, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_CATALOG_FLAG, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_PDF_FLAG, false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow(CREATE_OUTPUT_POSTERIORS_FLAG, false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Check that the constructor works fine if everything is OK
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(checkConstructorOK_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor with normal inputs");
  BOOST_TEST_MESSAGE(" ");

  // check
  BOOST_CHECK_NO_THROW(cf::ComputeRedshiftsConfiguration{options_map});

}

//-----------------------------------------------------------------------------
// Check that the constructor throws exception if we do not have write access to
// the phz-output-dir
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(constructorNoPhzOutputWriteAccess_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor with phz-output-dir without write access");
  BOOST_TEST_MESSAGE(" ");

  // Given
  fs::path no_write_dir = temp_dir.path() / "no_write";
  PhzUtils::createDirectoryIfAny(no_write_dir.string());
  options_map[PHZ_OUTPUT_DIR].value() = boost::any(no_write_dir.string());

  // When
  fs::permissions(no_write_dir, fs::perms::remove_perms|fs::perms::owner_write|fs::perms::others_write|fs::perms::group_write);

  // Then
  BOOST_CHECK_THROW(cf::ComputeRedshiftsConfiguration{options_map}, Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the constructor throws exception if the catalog contains filters
// that do not exist in the model grid
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(constructorMissingFilterInModelGrid_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor with model grid with missing filter");
  BOOST_TEST_MESSAGE(" ");

  // Given
  PhzDataModel::PhotometryGridInfo grid_info;
  grid_info.filter_names = {{"Filter1"}, {"Filter3"}};
  std::ofstream grid_out {model_grid.string()};
  boost::archive::binary_oarchive boa {grid_out};
  boa << grid_info;
  grid_out.close();

  // Then
  BOOST_CHECK_THROW(cf::ComputeRedshiftsConfiguration{options_map}, Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the constructor throws exception if the catalog contains filters
// that do not exist in the photometric correction file
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(constructorMissingFilterInPhotCorr_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor with photometric correction with missing filter");
  BOOST_TEST_MESSAGE(" ");

  // Given
    std::ofstream correction_file(phot_corr.string());
    // Fill up file
    correction_file << "#Filter Correction\n";
    correction_file << "#string     double\n";
    correction_file << "Filter1     1.1 \n";
    correction_file << "Filter3     2.2 \n";
    correction_file.close();

  // Then
  BOOST_CHECK_THROW(cf::ComputeRedshiftsConfiguration{options_map}, Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the getOutputHandler throws exception if no output is enabled
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerNoOutput_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler with no output enabled");
  BOOST_TEST_MESSAGE(" ");

  // When
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // Then
  BOOST_CHECK_THROW(conf.getOutputHandler(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the getOutputHandler throws exception for wrong output-catalog-format
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerWrongFormat_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler with wrong output-catalog-format");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[OUTPUT_CATALOG_FORMAT].value() = boost::any(std::string{"wrong"});

  // When
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // Then
  BOOST_CHECK_THROW(conf.getOutputHandler(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the getOutputHandler throws exception for wrong create-output-catalog
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerWronCatalogFlag_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler with wrong create-output-catalog");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_CATALOG_FLAG].value() = boost::any(std::string{"wrong"});

  // When
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // Then
  BOOST_CHECK_THROW(conf.getOutputHandler(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the getOutputHandler throws exception for wrong create-output-pdf
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerWrongPdfFlag_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler with wrong create-output-pdf");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_PDF_FLAG].value() = boost::any(std::string{"wrong"});

  // When
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // Then
  BOOST_CHECK_THROW(conf.getOutputHandler(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the getOutputHandler throws exception for wrong create-output-posteriors
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerWrongPosteriorsFlag_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler with wrong create-output-posteriors");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_POSTERIORS_FLAG].value() = boost::any(std::string{"wrong"});

  // When
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // Then
  BOOST_CHECK_THROW(conf.getOutputHandler(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that the catalog output is created when the create-output-catalog is YES
// for ascii catalog
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerCreateCatalogAscii_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler for catalog output");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_CATALOG_FLAG].value() = boost::any(std::string{"YES"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(fs::exists(phz_out_dir / "phz_cat.txt"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "pdf.fits"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "posteriors"));

}

//-----------------------------------------------------------------------------
// Check that the catalog output is created when the create-output-catalog is YES
// for fits catalog
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerCreateCatalogFits_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler for catalog output");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_CATALOG_FLAG].value() = boost::any(std::string{"YES"});
  options_map[OUTPUT_CATALOG_FORMAT].value() = boost::any(std::string{"FITS"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(fs::exists(phz_out_dir / "phz_cat.fits"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "pdf.fits"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "posteriors"));

}

//-----------------------------------------------------------------------------
// Check that the pdf output is created when the create-output-pdf is YES
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerCreatePdf_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler for pdf output");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_PDF_FLAG].value() = boost::any(std::string{"YES"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(!fs::exists(phz_out_dir / "phz_cat.txt"));
  BOOST_CHECK(fs::exists(phz_out_dir / "pdf.fits"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "posteriors"));

}

//-----------------------------------------------------------------------------
// Check that the posteriors output is created when the create-output-posteriors is YES
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputHandlerCreatePosteriors_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputHandler for posteriors output");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map[CREATE_OUTPUT_POSTERIORS_FLAG].value() = boost::any(std::string{"YES"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(!fs::exists(phz_out_dir / "phz_cat.txt"));
  BOOST_CHECK(!fs::exists(phz_out_dir / "pdf.fits"));
  BOOST_CHECK(fs::exists(phz_out_dir / "posteriors"));
  BOOST_CHECK(fs::exists(phz_out_dir / "posteriors" / "123.fits"));

}

//-----------------------------------------------------------------------------
// Check relative phz-output-dir
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(relativePhzOutputDir_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing for relative phz-output-dir");
  BOOST_TEST_MESSAGE(" ");

  // Given
  std::string relative_path {"relative/path"};
  options_map[PHZ_OUTPUT_DIR].value() = boost::any(std::string{"relative/path"});
  options_map[CREATE_OUTPUT_CATALOG_FLAG].value() = boost::any(std::string{"YES"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(fs::exists(results_dir / "CatalogType" / "input_catalog" / relative_path / "phz_cat.txt"));

}

//-----------------------------------------------------------------------------
// Check default phz-output-dir
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(defaultPhzOutputDir_test, ComputeRedshiftsConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing for relative phz-output-dir");
  BOOST_TEST_MESSAGE(" ");

  // Given
  options_map.erase(PHZ_OUTPUT_DIR);
  options_map[CREATE_OUTPUT_CATALOG_FLAG].value() = boost::any(std::string{"YES"});
  cf::ComputeRedshiftsConfiguration conf {options_map};

  // When
  {
    auto out_handler = conf.getOutputHandler();
    out_handler->handleSourceOutput(source, res);
  }

  // Then
  BOOST_CHECK(fs::exists(results_dir / "CatalogType" / "input_catalog" / "phz_cat.txt"));

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



