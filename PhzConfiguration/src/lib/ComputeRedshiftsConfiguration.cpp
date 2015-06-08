/**
 * @file ComputeRedshiftsConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Exception.h"
#include "PhzOutput/BestModelCatalog.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzConfiguration/ComputeRedshiftsConfiguration.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "PhzUtils/FileUtils.h"
#include "CheckPhotometries.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string AXES_COLLAPSE_TYPE {"axes-collapse-type"};
static const std::string OUTPUT_CATALOG_FORMAT {"output-catalog-format"};
static const std::string PHZ_OUTPUT_DIR {"phz-output-dir"};
static const std::string INPUT_CATALOG_FILE {"input-catalog-file"};
static const std::string CREATE_OUTPUT_CATALOG_FLAG {"create-output-catalog"};
static const std::string CREATE_OUTPUT_PDF_FLAG {"create-output-pdf"};
static const std::string CREATE_OUTPUT_POSTERIORS_FLAG {"create-output-posteriors"};

po::options_description ComputeRedshiftsConfiguration::getProgramOptions() {
  po::options_description options {"Compute Redshifts options"};

  options.add_options()
      (OUTPUT_CATALOG_FORMAT.c_str(), po::value<std::string>(),
          "The format of the PHZ catalog file (one of ASCII (default), FITS)")
      (PHZ_OUTPUT_DIR.c_str(), po::value<std::string>(),
          "The output directory of the PHZ results")
      (CREATE_OUTPUT_CATALOG_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
          "The output catalog flag for creating the file (YES/NO, default: NO)")
      (CREATE_OUTPUT_PDF_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
          "The output pdf flag for creating the file (YES/NO, default: NO)")
      (CREATE_OUTPUT_POSTERIORS_FLAG.c_str(), po::value<std::string>()->default_value("NO"),
           "The output posteriors flag for creating the file (YES/NO, default: NO)")
      (AXES_COLLAPSE_TYPE.c_str(), po::value<std::string>(),
        "The method used for collapsing the axes when producing the 1D PDF (one of SUM, MAX, BAYESIAN)");

  return merge(options)
              (PhotometricCorrectionConfiguration::getProgramOptions())
              (PhotometryCatalogConfiguration::getProgramOptions())
              (PhotometryGridConfiguration::getProgramOptions());
}

static fs::path getOutputPathFromOptions(const std::map<std::string, po::variable_value>& options,
                                         const fs::path& result_dir, const std::string& catalog_name) {
  fs::path input_catalog_name{options.at(INPUT_CATALOG_FILE).as<std::string>()};
  auto input_filename = input_catalog_name.filename().stem();
  fs::path result = result_dir / catalog_name / input_filename;
  if (options.count(PHZ_OUTPUT_DIR) > 0) {
    fs::path path = options.at(PHZ_OUTPUT_DIR).as<std::string>();
    if (path.is_absolute()) {
      result = path;
    } else {
      result = result_dir / catalog_name / input_filename / path;
    }
  }
  return result;
}

ComputeRedshiftsConfiguration::ComputeRedshiftsConfiguration(const std::map<std::string, po::variable_value>& options)
          : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options),
            CatalogConfiguration(options), PhotometryCatalogConfiguration(options),
            PhotometricCorrectionConfiguration(options), PhotometryGridConfiguration(options),
            PriorConfiguration() {
  m_options = options;

  auto output_dir = getOutputPathFromOptions(options, getResultsDir(), getCatalogName());

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryOnly(output_dir.string());

  // Check that the given grid contains photometries for all the filters we
  // have fluxes in the catalog
  checkGridPhotometriesMatch(getPhotometryGridInfo().filter_names,
                             getPhotometryFiltersToProcess());

  // Check that we have photometric corrections for all the filters
  checkHaveAllCorrections(getPhotometricCorrectionMap(),
                          getPhotometryFiltersToProcess());
}

class MultiOutputHandler : public PhzOutput::OutputHandler {
public:
	virtual ~MultiOutputHandler() = default;
	void addHandler(std::unique_ptr<PhzOutput::OutputHandler> handler) {
	  m_handlers.emplace_back(std::move(handler));
	}
	void handleSourceOutput(const SourceCatalog::Source& source,
	                                  const result_type& results) override {
		for (auto& handler : m_handlers) {
			handler->handleSourceOutput(source, results);
		}
	}
private:
	std::vector<std::unique_ptr<PhzOutput::OutputHandler>> m_handlers;
};

std::unique_ptr<PhzOutput::OutputHandler> ComputeRedshiftsConfiguration::getOutputHandler() {
  std::unique_ptr<MultiOutputHandler> result {new MultiOutputHandler{}};
  auto output_dir = getOutputPathFromOptions(m_options, getResultsDir(), getCatalogName());

  std::string cat_flag = m_options.count(CREATE_OUTPUT_CATALOG_FLAG) > 0
                     ? m_options.at(CREATE_OUTPUT_CATALOG_FLAG).as<std::string>()
                     : "NO";
  if (cat_flag == "YES") {
    auto format = PhzOutput::BestModelCatalog::Format::ASCII;
    auto out_catalog_file = output_dir / "phz_cat.txt";
    if (!m_options[OUTPUT_CATALOG_FORMAT].empty()) {
      auto format_str = m_options[OUTPUT_CATALOG_FORMAT].as<std::string>();
      if (format_str == "ASCII") {
        format = PhzOutput::BestModelCatalog::Format::ASCII;
      } else if (format_str == "FITS") {
        format = PhzOutput::BestModelCatalog::Format::FITS;
        out_catalog_file = output_dir / "phz_cat.fits";
      } else {
        throw Elements::Exception() << "Unknown output catalog format " << format_str;
      }
    }
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::BestModelCatalog{out_catalog_file.string(), format}});
  } else if (cat_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
                                << CREATE_OUTPUT_CATALOG_FLAG << " : " << cat_flag;
  }


  std::string pdf_flag = m_options.count(CREATE_OUTPUT_PDF_FLAG) > 0
                     ? m_options.at(CREATE_OUTPUT_PDF_FLAG).as<std::string>()
                     : "NO";
  if (pdf_flag == "YES") {
    auto out_pdf_file = output_dir / "pdf.fits";
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::PdfOutput{out_pdf_file.string()}});
  } else if (pdf_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
                                << CREATE_OUTPUT_PDF_FLAG << " : " << pdf_flag;
  }

  std::string post_flag = m_options.count(CREATE_OUTPUT_POSTERIORS_FLAG) > 0
                     ? m_options.at(CREATE_OUTPUT_POSTERIORS_FLAG).as<std::string>()
                     : "NO";
  if (post_flag == "YES") {
    auto out_post_file = output_dir / "posteriors";
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::LikelihoodHandler{out_post_file.string()}});
  } else if (post_flag != "NO") {
    throw Elements::Exception() << "Invalid value for option "
                                << CREATE_OUTPUT_POSTERIORS_FLAG << " : " << post_flag;
  }
  
  if (cat_flag == "NO" && pdf_flag == "NO" && post_flag == "NO") {
    throw Elements::Exception() << "At least one of the options " << CREATE_OUTPUT_CATALOG_FLAG
                                << ", " << CREATE_OUTPUT_PDF_FLAG << ", " << CREATE_OUTPUT_POSTERIORS_FLAG
                                << " must be set to YES";
  }

  return std::unique_ptr<PhzOutput::OutputHandler>{result.release()};
}

PhzLikelihood::SourcePhzFunctor::MarginalizationFunction ComputeRedshiftsConfiguration::getMarginalizationFunc() {
  if (m_options[AXES_COLLAPSE_TYPE].empty()) {
    throw Elements::Exception() << "Missing mandatory parameter " << AXES_COLLAPSE_TYPE;
  }
  if (m_options[AXES_COLLAPSE_TYPE].as<std::string>() == "SUM") {
    return PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  if (m_options[AXES_COLLAPSE_TYPE].as<std::string>() == "MAX") {
    return PhzLikelihood::MaxMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  if (m_options[AXES_COLLAPSE_TYPE].as<std::string>() == "BAYESIAN") {
    return PhzLikelihood::BayesianMarginalizationFunctor{};
  }
  throw Elements::Exception() << "Unknown " << AXES_COLLAPSE_TYPE << " \""
                    << m_options[AXES_COLLAPSE_TYPE].as<std::string>() << "\"";
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
