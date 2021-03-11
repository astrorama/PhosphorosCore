/**
 * @file tests/src/lib/LuminosityFunctionSet_test.cpp
 * @date 19 August 2015
 * @author Florian Dubath
 */


#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <memory>
#include <map>
#include <vector>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/FunctionAdapter.h"

#include "PhzLuminosity/LuminosityPrior.h"

using namespace Euclid;


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LuminosityPrior_test)

BOOST_AUTO_TEST_CASE(getMagFromFlux) {
  // Given
  std::vector<double> inputs{1.0, 3631E6, 3631E8};
  std::vector<double> expected{23.9, 0, -5};
  std::vector<double> results{};

  // When
  for (size_t index = 0; index < inputs.size(); ++index) {
    results.push_back(PhzLuminosity::LuminosityPrior::getMagFromFlux(inputs[index]));
  }

  // Then
  for (size_t index = 0; index < inputs.size(); ++index) {
    BOOST_CHECK_CLOSE(expected[index], results[index], 1E-3);
  }
}

BOOST_AUTO_TEST_CASE(getLuminosityInSample) {
  // Given
  std::vector<double> input_alpha        {1.0, 1.0, 1.0, 1.0};
  std::vector<double> input_n_sigma      {0.5, 0.5, 0.5, 0.5};
  std::vector<size_t> input_sample_number{11,   11,  11,  11};
  std::vector<size_t> input_sample_index {0,     5,  10,   1};

  std::vector<double> expected           {0.5, 1.0, 1.5, 0.6};

  std::vector<double> results{};

  // When
  for (size_t index = 0; index < input_alpha.size(); ++index) {
    results.push_back(PhzLuminosity::LuminosityPrior::getLuminosityInSample(input_alpha[index],
                                                                            input_n_sigma[index],
                                                                            input_sample_number[index],
                                                                            input_sample_index[index]));
  }

  // Then
  for (size_t index = 0; index < input_alpha.size(); ++index) {
    BOOST_CHECK_CLOSE(expected[index], results[index], 1E-3);
  }
}

BOOST_AUTO_TEST_CASE(createPriorGrid) {
  // Given
  std::vector<double> zs {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
                          1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1,
                          2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.2,
                          3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2, 4.3,
                          4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5.0, 5.1, 5.2, 5.3, 5.4,
                          5.5, 5.6, 5.7, 5.8, 5.9, 6.0};

  std::vector<double> ebvs {0.0, 0.1};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid posterior_grid{axes};

  PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
  PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
  grp_list["aa"] = sed_tp;


  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                        std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};


  PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions));

  // When
  auto result_grid = prior.createPriorGrid(posterior_grid);

  // Then
  auto z_axis = result_grid.getAxis<PhzDataModel::ModelParameter::Z>();
  BOOST_CHECK(z_axis.size() == zs.size());
  for (size_t index; index < z_axis.size(); ++index) {
    BOOST_CHECK(z_axis[index] == zs[index]);
  }

  for (auto iter = result_grid.begin(); iter != result_grid.end(); ++iter) {
    BOOST_CHECK(*iter == 0.);
  }

}

BOOST_AUTO_TEST_CASE(createListPriorGrid) {
  // Given
  std::vector<double> zs {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
                          1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1,
                          2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.2,
                          3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0, 4.1, 4.2, 4.3,
                          4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5.0, 5.1, 5.2, 5.3, 5.4,
                          5.5, 5.6, 5.7, 5.8, 5.9, 6.0};

  std::vector<double> ebvs {0.0, 0.1};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}, {"red_curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleListGrid posterior_grid{axes};
  for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
    for (size_t index=0; index<4;++index) {
      (*iter).push_back(1.1);
    }
  }

  PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
  PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
  grp_list["aa"] = sed_tp;


  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                        std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};


  PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions));

  // When
  auto result_grid = prior.createListPriorGrid(posterior_grid);

  // Then
  auto z_axis = result_grid.getAxis<PhzDataModel::ModelParameter::Z>();
  BOOST_CHECK(z_axis.size() == zs.size());
  for (size_t index; index < z_axis.size(); ++index) {
    BOOST_CHECK(z_axis[index] == zs[index]);
  }

  for (auto iter = result_grid.begin(); iter != result_grid.end(); ++iter) {
    BOOST_CHECK((*iter).size() == 4);
    for (size_t index=0; index<4;++index) {
      BOOST_CHECK((*iter)[index] == 0.);
    }
  }

}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


