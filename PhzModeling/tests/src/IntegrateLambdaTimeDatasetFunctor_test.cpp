/**
 * @file IntegrateDatasetFunctor_test.cpp
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/IntegrateLambdaTimeDatasetFunctor.h"
#include <boost/test/unit_test.hpp>
#include <cmath>

using std::make_pair;
using std::pair;
using std::vector;
using namespace Euclid;

struct IntegrateLambdaTimeDatasetFunctor_Fixture {
  double                       tolerance = 1E-3;
  double                       sigma     = 1.;
  double                       mean      = 1.;
  XYDataset::XYDataset         gaussian_dataset{gaussianDatasetValues()};
  vector<pair<double, double>> gaussianDatasetValues() {
    vector<pair<double, double>> result;
    for (double x = -1.; x <= 10; x += 0.1) {
      double value = std::exp(-1. * (x - mean) * (x - mean) / (2. * sigma * sigma)) / (sigma * std::sqrt(2. * M_PI));
      result.push_back(std::make_pair(x, value));
    }
    return result;
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(IntegrateLambdaTimeDatasetFunctor_test)

//-----------------------------------------------------------------------------
// Test the linear integration calculates correctly the sigmas of the distribution
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(linearIntegration, IntegrateLambdaTimeDatasetFunctor_Fixture) {

  // Given
  PhzModeling::IntegrateLambdaTimeDatasetFunctor functor{MathUtils::InterpolationType::LINEAR};

  // When
  double half_sigma      = functor(gaussian_dataset, make_pair(1., 1.5));
  double one_sigma       = functor(gaussian_dataset, make_pair(1.5, 2.));
  double onehalf_sigma   = functor(gaussian_dataset, make_pair(2., 2.5));
  double two_sigma       = functor(gaussian_dataset, make_pair(2.5, 3.));
  double twohalf_sigma   = functor(gaussian_dataset, make_pair(3., 3.5));
  double three_sigma     = functor(gaussian_dataset, make_pair(3.5, 4.));
  double threehalf_sigma = functor(gaussian_dataset, make_pair(4., 4.5));

  // Then
  BOOST_CHECK_LT(std::abs(half_sigma - 0.238), tolerance);
  BOOST_CHECK_LT(std::abs(one_sigma - 0.260), tolerance);
  BOOST_CHECK_LT(std::abs(onehalf_sigma - 0.204), tolerance);
  BOOST_CHECK_LT(std::abs(two_sigma - 0.120), tolerance);
  BOOST_CHECK_LT(std::abs(twohalf_sigma - 0.053), tolerance);
  BOOST_CHECK_LT(std::abs(three_sigma - 0.018), tolerance);
  BOOST_CHECK_LT(std::abs(threehalf_sigma - 0.005), tolerance);
}

//-----------------------------------------------------------------------------
// Test the linear integration uses zeroes outside the dataset range
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(linearIntegrationZeroes, IntegrateLambdaTimeDatasetFunctor_Fixture) {

  // Given
  PhzModeling::IntegrateLambdaTimeDatasetFunctor functor{MathUtils::InterpolationType::LINEAR};

  // When
  double first  = functor(gaussian_dataset, make_pair(-1., -0.5));
  double second = functor(gaussian_dataset, make_pair(-2., -0.5));

  // Then
  BOOST_CHECK_LT(std::abs(first + 0.031), tolerance);
  BOOST_CHECK_LT(std::abs(second + 0.031), tolerance);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
