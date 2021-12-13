/**
 * @file tests/src/serialization/PhotometryGrid_test.cpp
 * @date Sep 24, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzDataModel/Photometry.h"

struct PhzPhotometryGridName_Fixture {

  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2"});
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2","filter3"});
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter3"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_5{{1.5,2.5},{3.5,4.5},{5.5,6.5}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};
  Euclid::SourceCatalog::Photometry photometry_5{filter_2,values_5};
  Euclid::SourceCatalog::Photometry photometry_6{filter_3,values_1};

  PhzPhotometryGridName_Fixture(){

  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryGridSerialization_test)

template<typename I, typename O>
struct archive {
  typedef I iarchive;
  typedef O oarchive;
};

typedef archive<boost::archive::binary_iarchive, boost::archive::binary_oarchive> binary_archive;
typedef archive<boost::archive::text_iarchive, boost::archive::text_oarchive> text_archive;

typedef boost::mpl::list<binary_archive, text_archive> archive_types;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(serializationException_test, PhzPhotometryGridName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the Exception throw by the serialization of the Photometry Grid");
  BOOST_TEST_MESSAGE(" ");

  std::stringstream stream;
  boost::archive::text_oarchive oa(stream);

  auto axes=Euclid::PhzDataModel::createAxesTuple({},ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid empty_grid(axes, *filter_1);
  Euclid::PhzDataModel::PhotometryGrid *grid_ptr=&empty_grid;
  // Get the empty grid exception
  BOOST_CHECK_THROW((oa << grid_ptr),Elements::Exception);

  //---------------------------------------------------------------

  axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid(axes, *filter_1);

  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_5;

  std::stringstream stream2;
  boost::archive::text_oarchive oa2(stream2);
  grid_ptr=&original_grid;
  // Get the exception for not identical filter list (not the same number)
  BOOST_CHECK_THROW((oa2 << grid_ptr),Elements::Exception);
  //---------------------------------------------------------------

  Euclid::PhzDataModel::PhotometryGrid original_grid_2{axes, *filter_1};
  original_grid_2(0,0,0,0)=photometry_1;
  original_grid_2(1,0,0,0)=photometry_2;
  original_grid_2(0,1,0,0)=photometry_3;
  original_grid_2(1,1,0,0)=photometry_6;
  std::stringstream stream3;
  boost::archive::text_oarchive oa3(stream3);
  grid_ptr=&original_grid_2;
  // Get the exception for not identical filter list (not the same name)
  BOOST_CHECK_THROW((oa3 << grid_ptr),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(serialization_test, T, archive_types, PhzPhotometryGridName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the serialization of the Photometry Grid");
  BOOST_TEST_MESSAGE(" ");

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes, *filter_1};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;

  std::stringstream stream;
  Euclid::GridContainer::gridExport<typename T::oarchive>(stream, original_grid);
  auto retrived_grid = Euclid::GridContainer::gridImport<Euclid::PhzDataModel::PhotometryGrid, typename T::iarchive>(stream);

  //-------------------------------------------------------------------
  BOOST_CHECK_EQUAL(original_grid.size(), retrived_grid.size());
  // Check values
  for(int v_index=0;v_index==2;v_index++){
    for(int evb_index=0;evb_index==2;evb_index++){
      auto expected_photometry = original_grid(v_index,evb_index,0,0);
      auto actual_photometry = retrived_grid(v_index,evb_index,0,0);

      auto expected_iterator = expected_photometry.begin();
      for(auto actual_iterator : actual_photometry){
        BOOST_CHECK(Elements::isEqual((*expected_iterator).flux,actual_iterator.flux));
        BOOST_CHECK(Elements::isEqual((*expected_iterator).error,actual_iterator.error));
        ++expected_iterator;
      }
    }
  }

  // check filter names
  for(int v_index=0;v_index==2;v_index++){
     for(int evb_index=0;evb_index==2;evb_index++){
       auto expected_photometry = original_grid(v_index,evb_index,0,0);
       auto actual_photometry = retrived_grid(v_index,evb_index,0,0);
       auto original_filters = original_grid.getCellManager().filterNames();
       auto actual_filters =  retrived_grid.getCellManager().filterNames();
       BOOST_CHECK_EQUAL_COLLECTIONS(original_filters.begin(), original_filters.end(), actual_filters.begin(), actual_filters.end());
     }
   }
}


BOOST_FIXTURE_TEST_CASE(toTable_test, PhzPhotometryGridName_Fixture) {
  auto axes = Euclid::PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes, *filter_1};
  original_grid(0, 0, 0, 0) = photometry_1;
  original_grid(1, 0, 0, 0) = photometry_2;
  original_grid(0, 1, 0, 0) = photometry_3;
  original_grid(1, 1, 0, 0) = photometry_4;

  auto table = Euclid::GridContainer::gridContainerToTable(original_grid);
  auto column_info = table.getColumnInfo();

  BOOST_CHECK_EQUAL(table.size(), 4);
  BOOST_CHECK_EQUAL(column_info->size(), 8);
  BOOST_CHECK(column_info->getDescription("Z").type == typeid(double));
  BOOST_CHECK(column_info->getDescription("E(B-V)").type == typeid(double));
  BOOST_CHECK(column_info->getDescription("filtre1").type == typeid(double));
  BOOST_CHECK(column_info->getDescription("filtre1_error").type == typeid(double));
  BOOST_CHECK(column_info->getDescription("filter2").type == typeid(double));
  BOOST_CHECK(column_info->getDescription("filter2_error").type == typeid(double));

  // Note that QualifiedName must be translated into std::string when writing the table
  BOOST_CHECK(column_info->getDescription("Reddening_Curve").type == typeid(std::string));
  BOOST_CHECK(column_info->getDescription("SED").type == typeid(std::string));

  // Verify values
  for (auto& row: table) {
    auto z = boost::get<double>(row["Z"]);
    auto ebv = boost::get<double>(row["E(B-V)"]);
    auto reddening = Euclid::XYDataset::QualifiedName(boost::get<std::string>(row["Reddening_Curve"]));
    auto sed = Euclid::XYDataset::QualifiedName(boost::get<std::string>(row["SED"]));

    auto z_idx = std::distance(zs.begin(), std::find(zs.begin(), zs.end(), z));
    auto ebv_idx = std::distance(ebvs.begin(), std::find(ebvs.begin(), ebvs.end(), ebv));
    auto r_idx = std::distance(reddeing_curves.begin(), std::find(reddeing_curves.begin(), reddeing_curves.end(), reddening));
    auto s_idx = std::distance(seds.begin(), std::find(seds.begin(), seds.end(), sed));

    auto cell = original_grid.at(z_idx, ebv_idx, r_idx, s_idx);

    for (auto fname : *filter_1) {
      auto flux = boost::get<double>(row[fname]);
      auto error = boost::get<double>(row[fname + "_error"]);

      auto gflux = cell.find(fname);
      BOOST_CHECK_EQUAL(gflux->flux, flux);
      BOOST_CHECK_EQUAL(gflux->error, error);
    }
  }
}


BOOST_AUTO_TEST_SUITE_END ()
