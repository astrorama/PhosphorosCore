/**
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file tests/src/FilterVariationSingleGridCreator_test.cpp
 * @date 2021/09/06
 * @author Florian Dubath
 */

#include <PhzModeling/BuildFilterInfoFunctor.h>
#include <boost/test/unit_test.hpp>
#include <set>
#include <string>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
struct FilterVariationSingleGridCreator_Fixture {


};





//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FilterVariationSingleGridCreator_test)


BOOST_FIXTURE_TEST_CASE(shifFilter_test, FilterVariationSingleGridCreator_Fixture) {
   std::vector<double> x{1.0, 2.0, 3.0, 4.0};
   std::vector<double> y{0.1, 0.2, 0.3, 0.4};
   XYDataset::XYDataset filter = XYDataset::XYDataset::factory(x, y);

   auto result = PhzFilterVariation::FilterVariationSingleGridCreator::shiftFilter(filter, 0.3);

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
  PhzModeling::BuildFilterInfoFunctor filter_info_functor;

  std::vector<double> delta_lambda{-100, -10, 0, 10, 100};
  auto                filter_dataset = XYDataset::XYDataset::factory(lambda, filter_val);
  auto                filter         = filter_info_functor(filter_dataset);
  auto                sed            = XYDataset::XYDataset::factory(lambda, sed_val);

  std::vector<PhzDataModel::FilterInfo> shifted_filter;
  for (auto dl : delta_lambda) {
    shifted_filter.emplace_back(
        filter_info_functor(PhzFilterVariation::FilterVariationSingleGridCreator::shiftFilter(filter_dataset, dl)));
  }

  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateLambdaTimeDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  std::vector<double> expected{0.68796, 0.96781411, 1, 1.032406, 1.3340858};

  auto res = PhzFilterVariation::FilterVariationSingleGridCreator::compute_coef(sed,
      filter, shifted_filter, filter_functor, integrate_dataset_function);

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
  PhzModeling::BuildFilterInfoFunctor filter_info_functor;

  std::vector<double> delta_lambda{-100, -10, -1, 0, 1, 10, 100};
  auto                filter_dataset = XYDataset::XYDataset::factory(lambda, filter_val);
  auto                filter         = filter_info_functor(filter_dataset);
  auto                sed            = XYDataset::XYDataset::factory(lambda, sed_val);

  std::vector<PhzDataModel::FilterInfo> shifted_filter;
  for (auto dl : delta_lambda) {
    shifted_filter.emplace_back(
        filter_info_functor(PhzFilterVariation::FilterVariationSingleGridCreator::shiftFilter(filter_dataset, dl)));
  }

  auto filter_functor = PhzModeling::ApplyFilterFunctor();
  auto integrate_dataset_function = PhzModeling::IntegrateLambdaTimeDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  std::vector<double> expected{0.003120346, 0.0032185, 0.0032285, 0., 0.0032307, 0.003240640, 0.003340858};

  auto res = PhzFilterVariation::FilterVariationSingleGridCreator::compute_tild_coef(sed, filter, shifted_filter, delta_lambda,
                                                                                     filter_functor, integrate_dataset_function);

  for (size_t i = 0; i < expected.size(); ++i) {
      BOOST_CHECK_CLOSE(res[i], expected[i], 0.01);
  }
}

BOOST_AUTO_TEST_SUITE_END ()

}
