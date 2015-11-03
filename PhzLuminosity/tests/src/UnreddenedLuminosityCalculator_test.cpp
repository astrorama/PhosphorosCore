/*
 * UnreddenedLuminosityCalculator_test.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <cmath>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>
#include "PhzDataModel/PhotometryGrid.h"

#include "PhzLuminosity/UnreddenedLuminosityCalculator.h"

using namespace Euclid;

struct UnreddenedLuminosityCalculator_Fixture {
  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

    std::vector<double> zs{0.0,0.2,0.6,1.0};
    std::map<double,double> luminosity_distance_map{};
    std::map<double,double> distance_modulus_map{};

    std::vector<double> ebvs{0.0,0.1,0.2};
    std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
    std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve_1"},{"sed/Curve_2"},{"sed/Curve_3"}};

    Euclid::PhzDataModel::ModelAxesTuple parameter_space= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

    // Create the scale factor Grid
    PhzDataModel::ScaleFactordGrid scale_factor_grid{parameter_space};

    std::vector<double> zs_mod{0.0};
    Euclid::PhzDataModel::ModelAxesTuple parameter_space_slice= Euclid::PhzDataModel::createAxesTuple(zs_mod,ebvs,reddeing_curves,seds);

    PhzDataModel::PhotometryGrid model_grid {parameter_space_slice};


    // Shared pointer of filter name vector
    std::shared_ptr<std::vector<std::string>> filter_name_vector_ptr {
        new std::vector<std::string> { luminosityFilterName.qualifiedName() ,"Another Name"  } };

    // photometry values vector
    std::vector<SourceCatalog::FluxErrorPair> photometry_vector { SourceCatalog::FluxErrorPair(1.0,
        0.1), SourceCatalog::FluxErrorPair(2.0, 0.3) };




    UnreddenedLuminosityCalculator_Fixture(){


      double value = 1.;
      for (auto& photometry : model_grid){
        std::vector<SourceCatalog::FluxErrorPair> photometry_vector { SourceCatalog::FluxErrorPair(value,
             0.1), SourceCatalog::FluxErrorPair(0.1, 0.3) };
        photometry = SourceCatalog::Photometry{filter_name_vector_ptr,photometry_vector};
        value+=1.;
      }

      value = 1.3;
      for (auto& scale_factor : scale_factor_grid){
        scale_factor = value;
        value+=1.;
      }
    }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityCalculator_test)

//---------------------------------------------------------------------------
/**
 * Check that the operator() select the right model into the luminosity model grid
 */
BOOST_FIXTURE_TEST_CASE(test_mag, UnreddenedLuminosityCalculator_Fixture) {
  std::shared_ptr<PhzDataModel::PhotometryGrid> model_grid_ptr {
       new PhzDataModel::PhotometryGrid { std::move(model_grid) } };

   PhzLuminosity::UnreddenedLuminosityCalculator lum_comp_funct {
       luminosityFilterName, model_grid_ptr, luminosity_distance_map,
       distance_modulus_map, true };
   auto scale_iter = scale_factor_grid.cbegin();
   while (scale_iter != scale_factor_grid.cend()) {

     auto sed = scale_iter.axisValue<PhzDataModel::ModelParameter::SED>();
     auto red = scale_iter.axisValue<
         PhzDataModel::ModelParameter::REDDENING_CURVE>();

     auto computed = lum_comp_funct.fixIterator(scale_iter, sed);

     double c_z = computed.axisValue<PhzDataModel::ModelParameter::Z>();
     auto c_sed = computed.axisValue<PhzDataModel::ModelParameter::SED>();
     double c_ebv = computed.axisValue<PhzDataModel::ModelParameter::EBV>();
     auto c_red = computed.axisValue<
         PhzDataModel::ModelParameter::REDDENING_CURVE>();

     BOOST_CHECK_EQUAL(c_z, 0.);
     BOOST_CHECK_EQUAL(c_sed.qualifiedName(), sed.qualifiedName());
     BOOST_CHECK_EQUAL(c_ebv, 0.);
     BOOST_CHECK_EQUAL(c_red.qualifiedName(), red.qualifiedName());

     ++scale_iter;
   }
}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


