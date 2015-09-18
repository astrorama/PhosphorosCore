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

struct LuminosityCalculator_Fixture {


  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

  PhysicsUtils::Cosmology cosmology{};

  std::vector<double> zs{0.0,0.2,0.6,1.0};
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




  LuminosityCalculator_Fixture(){


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



  double getLuminosity(
      PhzLuminosity::UnreddenedLuminosityCalculator& calc,
      const PhzDataModel::PhotometryGrid::const_iterator& model,
      double scaleFactor,
      double z){

    return calc.getLuminosityFromModel(model,scaleFactor,z);
  }



};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityCalculator_test)

//---------------------------------------------------------------------------
/**
 * Check that the operator() select the right model into the luminosity model grid
 */
BOOST_FIXTURE_TEST_CASE(test_mag, LuminosityCalculator_Fixture) {
  std::shared_ptr<PhzDataModel::PhotometryGrid>  model_grid_ptr{new PhzDataModel::PhotometryGrid{std::move(model_grid)}};

  PhzLuminosity::UnreddenedLuminosityCalculator lum_comp_funct{luminosityFilterName,model_grid_ptr,true};

    auto scale_iter = scale_factor_grid.cbegin();
    while (scale_iter != scale_factor_grid.cend()){
      double alpha = *scale_iter;
      double z = scale_iter.axisValue<PhzDataModel::ModelParameter::Z>();
      auto sed  = scale_iter.axisValue<PhzDataModel::ModelParameter::SED>();
      double ebv  = scale_iter.axisValue<PhzDataModel::ModelParameter::EBV>();
      auto red  = scale_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>();

      double computed = lum_comp_funct(scale_iter,z,sed);

      auto model_iter = model_grid_ptr->begin();
      model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);
      model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(red);
      model_iter.fixAxisByValue<PhzDataModel::ModelParameter::EBV>(0.);
      model_iter.fixAxisByValue<PhzDataModel::ModelParameter::Z>(0.);

      double expected = getLuminosity(lum_comp_funct,model_iter,alpha,z);

      BOOST_CHECK(Elements::isEqual(computed,expected));


      ++scale_iter;
    }

}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


