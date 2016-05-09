/**
 * @file tests/src/lib/LuminosityCalculator_test.cpp
 * @date August 31, 2015
 * @author Florian dubath
 */

#include <memory>
#include <vector>
#include <cmath>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>
#include "PhzDataModel/PhotometryGrid.h"
#include "PhysicsUtils/CosmologicalDistances.h"

#include "PhzLuminosity/LuminosityCalculator.h"

using namespace Euclid;



struct LuminosityCalculator_Fixture {

  class TestLuminosityCalculator : public PhzLuminosity::LuminosityCalculator{
  public:

    const PhzDataModel::PhotometryGrid::const_iterator fixIterator(
           const PhzDataModel::ScaleFactordGrid::const_iterator&) const override{
      return m_model_photometry_grid->cbegin();
    }

    TestLuminosityCalculator(
        XYDataset::QualifiedName luminosity_filter,
        std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
        std::map<double,double> luminosity_distance_map,
        std::map<double,double> distance_modulus_map,
        bool in_mag)
    : LuminosityCalculator(
        luminosity_filter,
        model_photometry_grid,
        luminosity_distance_map,
        distance_modulus_map,
        in_mag) { }
  };


  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

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
    /* fill the Model_grid: the model grid has axies :
     *  z={0.0}
     *  ebv={0.0,0.1,0.2}
     *  reddening curve = {reddeningCurves/Curve1}
     *  seds = {ed/Curve_1,sed/Curve_2,sed/Curve_3}
     *  alltogether it is 1x3x1x3=9 models
     *
     *  the grid is filled with photometries values with the first photometrie
     *  matching the luminosityFilterName
     *  {{1.7,0.1}{0.1,0.3}}     ebv=0 SED=curve_1
     *  {{2.7,0.1}{0.1,0.3}}     ebv=0.1 SED=curve_1
     *  {{3.7,0.1}{0.1,0.3}}     ebv=0.2 SED=curve_1
     *
     *  {{4.7,0.1}{0.1,0.3}}     ebv=0 SED=curve_2
     *  {{5.7,0.1}{0.1,0.3}}     ...
     *  {{6.7,0.1}{0.1,0.3}}
     *
     *  {{7.7,0.1}{0.1,0.3}}     ebv=0 SED=curve_3
     *  {{8.7,0.1}{0.1,0.3}}
     *  {{9.7,0.1}{0.1,0.3}}
     *
     *  the scale factor grid has the same axis but the z which
     *  contains {0.0,0.2,0.6,1.0}and is filled with values
     *  1.3, 2.3, 3.3, 4.3, ...
     *  Z=0 values are thus 1.3, 5.3,...
     *
     */

   double value = 1.7;
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



/**
 * Check the Luminosity in flux
 */
BOOST_FIXTURE_TEST_CASE(test_flux, LuminosityCalculator_Fixture) {
  std::shared_ptr<PhzDataModel::PhotometryGrid> model_grid_ptr {
       new PhzDataModel::PhotometryGrid { std::move(model_grid) } };

std::map<double,double> distance_correction{{0.0,0.},{0.2,100.},{0.6,1000000.},{1.0,10000000000.}};
std::map<double,double> flux_values{{0.0,2644265000.0},{0.2,3898595833.33},{0.6,4195228125.0},{1.0,4373207500.0}};


TestLuminosityCalculator lum_comp_funct {
   luminosityFilterName,
   model_grid_ptr,
   distance_correction,
   {},
   false };
auto scale_iter = scale_factor_grid.cbegin();
int loop=0;
while (scale_iter != scale_factor_grid.cend() && loop<4) {

 double z = scale_iter.axisValue<PhzDataModel::ModelParameter::Z>();


 auto computed = lum_comp_funct(scale_iter);
 auto expected =flux_values.at(z)*distance_correction.at(z)*distance_correction.at(z);

 BOOST_CHECK_CLOSE(computed,expected,1E-2); // check at 1e-2 % => 1e-4 which is the precision expected from the provided flux_values
 ++scale_iter;
 ++loop;
}
}

/**
 * Check the Luminosity in magnitude
 */
BOOST_FIXTURE_TEST_CASE(test_mag, LuminosityCalculator_Fixture) {
  std::shared_ptr<PhzDataModel::PhotometryGrid> model_grid_ptr {
      new PhzDataModel::PhotometryGrid { std::move(model_grid) } };

  std::map<double,double> modulus_correction{{0.0,0.},{0.2,1.},{0.6,10.},{1.0,100.}};
  std::map<double,double> log_lum{{0.0,23.0390849},{0.2,22.6175768},{0.6,22.53795},{1.0,22.49285}};

  TestLuminosityCalculator lum_comp_funct {
      luminosityFilterName,
      model_grid_ptr,
      {},
      modulus_correction,
      true };
  auto scale_iter = scale_factor_grid.cbegin();
  int loop=0;
  while (scale_iter != scale_factor_grid.cend() && loop<4) {

    double z = scale_iter.axisValue<PhzDataModel::ModelParameter::Z>();

    auto computed = lum_comp_funct(scale_iter);
    auto expected = log_lum.at(z)-modulus_correction.at(z);

    BOOST_CHECK_CLOSE(computed,expected,1E-2);
    ++scale_iter;
    ++loop;
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


