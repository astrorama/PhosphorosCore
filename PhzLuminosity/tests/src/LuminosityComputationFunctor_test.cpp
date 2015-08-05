/*
 * LuminosityComputationFunctor_Test.cpp
 *
 *  Created on: Aug 3, 2015
 *      Author: fdubath
 */

#include <memory>
#include <vector>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>
#include "PhzLuminosity/LuminosityComputationFunctor.h"
#include "PhzDataModel/PhotometryGrid.h"

using namespace Euclid;

struct LuminosityComputationFunctor_Fixture {
  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

  std::vector<double> zs{0.0,0.2,0.6,1.0};
  std::vector<double> ebvs{0.0,0.01};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"},{"reddeningCurves/Curve2"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve_1"},{"sed/Curve_2"},{"sed/Curve_3"}};

  Euclid::PhzDataModel::ModelAxesTuple parameter_space= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  // Create the photometry Grid

  PhzDataModel::PhotometryGrid photometry_grid{parameter_space};

  std::vector<double> zs_mod{0.0};
  Euclid::PhzDataModel::ModelAxesTuple parameter_space_slice= Euclid::PhzDataModel::createAxesTuple(zs_mod,ebvs,reddeing_curves,seds);

  PhzDataModel::PhotometryGrid model_grid {parameter_space_slice};


  // Shared pointer of filter name vector
  std::shared_ptr<std::vector<std::string>> filter_name_vector_ptr {
      new std::vector<std::string> { luminosityFilterName.qualifiedName() ,"Another Name"  } };

  // photometry values vector
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector { SourceCatalog::FluxErrorPair(1.0,
      0.1), SourceCatalog::FluxErrorPair(2.0, 0.3) };


  LuminosityComputationFunctor_Fixture(){


    for (auto& photometry : photometry_grid){
      photometry = SourceCatalog::Photometry{filter_name_vector_ptr, photometry_vector};
    }

    for (auto& photometry : model_grid){
      photometry = SourceCatalog::Photometry{filter_name_vector_ptr,photometry_vector};
    }
  }



};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityComputationFunctor_Test)

//---------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_calling, LuminosityComputationFunctor_Fixture) {
  PhzLuminosity::LuminosityComputationFunctor lumComp_funct{luminosityFilterName};

  auto phot_grid_iter = photometry_grid.begin();
  while (phot_grid_iter != photometry_grid.end()){
    PhzLuminosity::GridCoordinate coordinate{
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::Z>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::EBV>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::SED>()
    };

    lumComp_funct(coordinate,1.0,phot_grid_iter,model_grid);

    ++phot_grid_iter;
  }
}

BOOST_FIXTURE_TEST_CASE(test_value_1_flux, LuminosityComputationFunctor_Fixture) {
  PhzLuminosity::LuminosityComputationFunctor lumComp_funct{luminosityFilterName};
  PhysicsUtils::Cosmology cosmology{};
  auto phot_grid_iter = photometry_grid.begin();
  double scale_factor = 1.0;
  while (phot_grid_iter != photometry_grid.end()){
    double z = phot_grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
    PhzLuminosity::GridCoordinate coordinate{
      z,
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::EBV>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::SED>()
    };

    double m = lumComp_funct(coordinate,scale_factor,phot_grid_iter,model_grid);
    m += cosmology.DistanceModulus(z);
    m /= -2.5;
    m -= std::log10(scale_factor);
    BOOST_CHECK(std::abs(std::log10(phot_grid_iter->find(luminosityFilterName.qualifiedName())->flux)-m)<1E-14);

    scale_factor+=9.99;
    ++phot_grid_iter;
  }
}

BOOST_FIXTURE_TEST_CASE(test_missing_z0, LuminosityComputationFunctor_Fixture) {
  PhzLuminosity::LuminosityComputationFunctor lumComp_funct{luminosityFilterName};

  std::vector<double> zs_model{1.0};
  Euclid::PhzDataModel::ModelAxesTuple m_parameter_space_slice= Euclid::PhzDataModel::createAxesTuple(zs_model,ebvs,reddeing_curves,seds);

  PhzDataModel::PhotometryGrid m_model_grid{m_parameter_space_slice};
  for (auto& photometry : m_model_grid){
      photometry = SourceCatalog::Photometry{filter_name_vector_ptr,photometry_vector};
  }


  auto phot_grid_iter = photometry_grid.begin();
  while (phot_grid_iter != photometry_grid.end()){
    PhzLuminosity::GridCoordinate coordinate{
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::Z>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::EBV>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::SED>()
    };

    BOOST_CHECK_THROW(lumComp_funct(coordinate,1.0,phot_grid_iter,m_model_grid),Elements::Exception);

    ++phot_grid_iter;
  }
}

BOOST_FIXTURE_TEST_CASE(test_missing_filter, LuminosityComputationFunctor_Fixture) {
  PhzLuminosity::LuminosityComputationFunctor lumComp_funct{XYDataset::QualifiedName{"toto"}};

  auto phot_grid_iter = photometry_grid.begin();
  while (phot_grid_iter != photometry_grid.end()){
    PhzLuminosity::GridCoordinate coordinate{
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::Z>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::EBV>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(),
      phot_grid_iter.axisValue<PhzDataModel::ModelParameter::SED>()
    };

    BOOST_CHECK_THROW(lumComp_funct(coordinate,1.0,phot_grid_iter,model_grid),Elements::Exception);

    ++phot_grid_iter;
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


