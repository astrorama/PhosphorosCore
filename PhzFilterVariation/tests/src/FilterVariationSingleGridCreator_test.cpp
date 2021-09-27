/**
 * @file tests/src/FilterVariationSingleGridCreator_test.cpp
 * @date 2021/09/06
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <vector>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"


namespace Euclid {
struct FilterVariationSingleGridCreator_Fixture {


};





//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FilterVariationSingleGridCreator_test)


BOOST_FIXTURE_TEST_CASE(shifFilter_test, FilterVariationSingleGridCreator_Fixture) {
   std::vector<double> x{1.0, 2.0, 3.0, 4.0};
   std::vector<double> y{0.1, 0.2, 0.3, 0.4};
   XYDataset::XYDataset filter = XYDataset::XYDataset::factory(x, y);

   auto result = PhzFilterVariation::FilterVariationSingleGridCreator::shifFilter(filter, 0.3);

   auto iter = result.begin();
   for (size_t i = 0; i < x.size(); ++i) {
     BOOST_CHECK_CLOSE(iter->second, y[i], 0.001);
     BOOST_CHECK_CLOSE(iter->first, x[i] + 0.3, 0.001);
     ++iter;
   }
}

BOOST_FIXTURE_TEST_CASE(computeSampling_test, FilterVariationSingleGridCreator_Fixture) {
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(20, -20, 10), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(20, 20, 10), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-20, 20, -1), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-20, 20, 0), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-20, 20, 1), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-20, 20, 2), Elements::Exception);
  BOOST_CHECK_THROW(PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-20, 20, 3), Elements::Exception);


  std::vector<double> expected{-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = PhzFilterVariation::FilterVariationSingleGridCreator::computeSampling(-10, 10, 21);

  auto iter = result.begin();
   for (size_t i = 0; i < expected.size(); ++i) {
     BOOST_CHECK_CLOSE(*iter, expected[i], 0.001);
     ++iter;
   }
}


BOOST_FIXTURE_TEST_CASE(compute_coef_test, FilterVariationSingleGridCreator_Fixture) {
  std::vector<double> lambda{};
  std::vector<double> sed_val{};
  std::vector<double> filter_val{};
  for (int index = 0; index < 701; ++index) {
    lambda.push_back(5000+index);
    sed_val.push_back(index);
    if (index < 300 || index > 400) {
      filter_val.push_back(0);
    } else {
      filter_val.push_back(1);
    }
  }

  // nominal flux = int_300^400 x dx = 1/2x² |_300^400 = 35000
  std::vector<double> delta_lambda{-100, -10, 0, 10, 100};
  auto filter = XYDataset::XYDataset::factory(lambda, filter_val);
  auto sed = XYDataset::XYDataset::factory(lambda, sed_val);

  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  std::vector<double> expected{0.7142857,0.9714286,1,1.0285714,1.2857143};

  auto res = PhzFilterVariation::FilterVariationSingleGridCreator::compute_coef(sed,
      filter, delta_lambda, filter_functor, integrate_dataset_function);

  for (size_t i = 0; i < expected.size(); ++i) {
      BOOST_CHECK_CLOSE(res[i], expected[i], 0.001);
  }
}

BOOST_FIXTURE_TEST_CASE(compute_tilde_coef_test, FilterVariationSingleGridCreator_Fixture) {
  std::vector<double> lambda{};
  std::vector<double> sed_val{};
  std::vector<double> filter_val{};
  for (int index = 0; index < 701; ++index) {
    lambda.push_back(5000+index);
    sed_val.push_back(index);
    if (index < 300 || index > 400) {
      filter_val.push_back(0);
    } else {
      filter_val.push_back(1);
    }
  }

  // nominal flux = int_300^400 x dx = 1/2x² |_300^400 = 35000
  std::vector<double> delta_lambda{-100, -10, -1, 0, 1, 10, 100};
  auto filter = XYDataset::XYDataset::factory(lambda, filter_val);
  auto sed = XYDataset::XYDataset::factory(lambda, sed_val);

  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  std::vector<double> expected{0.0028571,0.0028571,0.0028571,0,0.0028571,0.0028571,0.0028571};

  auto res = PhzFilterVariation::FilterVariationSingleGridCreator::compute_tild_coef(sed,
      filter, delta_lambda, filter_functor, integrate_dataset_function);

  for (size_t i = 0; i < expected.size(); ++i) {
      BOOST_CHECK_CLOSE(res[i], expected[i], 0.01);
  }
}

BOOST_FIXTURE_TEST_CASE(regression_test, FilterVariationSingleGridCreator_Fixture) {
	  std::vector<double> delta_lambda{-100,-50,-20,-10,10,20,50,100};
	  std::vector<double> tild_coef{1,1.5,1.8,1.9,2.1,2.2,2.5,3};

	  double expected_a = 0.01;
	  double expected_b = 2.0;

	  auto res = PhzFilterVariation::FilterVariationSingleGridCreator::do_regression(delta_lambda, tild_coef);

	  BOOST_CHECK_CLOSE(res.first, expected_a, 0.01);
	  BOOST_CHECK_CLOSE(res.second, expected_b, 0.01);
}



BOOST_AUTO_TEST_SUITE_END ()

}
