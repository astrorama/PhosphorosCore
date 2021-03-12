/**
 * @file tests/src/lib/LuminosityFunctionSet_test.cpp
 * @date 19 August 2015
 * @author Florian Dubath
 */


#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzLuminosity/LuminosityFunctionSet.h"
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/function/FunctionAdapter.h"
#include "MathUtils/function/Polynomial.h"

#include "PhzLuminosity/LuminosityPrior.h"

using namespace Euclid;

double test_lum(double) {
   return 1.0;
}



double test_lum_mirror(double flux) {
   return flux;
 }

double test_lum_abs(double mag) {
   return std::abs(mag);
 }

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE (LuminosityGroupdProcessor)

BOOST_AUTO_TEST_CASE(constructor) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid posterior_grid{axes};
  PhzDataModel::DoubleGrid scale_factor_grid{axes};

  // When
  PhzLuminosity::LuminosityPrior::LuminosityGroupdProcessor proc(posterior_grid, scale_factor_grid, true);

  // Then
  BOOST_CHECK_CLOSE(0.0, proc.getMaxPrior(), 1E-3);
}


BOOST_AUTO_TEST_CASE(operator_function) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid prior_grid{axes};
  PhzDataModel::DoubleGrid scale_factor_grid{axes};
  PhzLuminosity::LuminosityPrior::LuminosityGroupdProcessor proc(prior_grid, scale_factor_grid, false);

  // When
  proc(test_lum, 0, 0);

  // Then
  auto iter = prior_grid.begin();
  BOOST_CHECK_CLOSE(1.0, (*iter), 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter), 1E-3);
  BOOST_CHECK_CLOSE(1.0, proc.getMaxPrior(), 1E-3);
}


BOOST_AUTO_TEST_CASE(operator_function_flux) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid prior_grid{axes};
  PhzDataModel::DoubleGrid scale_factor_grid{axes};

  auto scale_iter = scale_factor_grid.begin();
  (*scale_iter) = 5;
  ++scale_iter;
  (*scale_iter) = 4;
  ++scale_iter;
  (*scale_iter) = 3;
  ++scale_iter;
  (*scale_iter) = 2;
  ++scale_iter;
  (*scale_iter) = 1;
  ++scale_iter;
  (*scale_iter) = 0;

  PhzLuminosity::LuminosityPrior::LuminosityGroupdProcessor proc(prior_grid, scale_factor_grid, false);

  // When
  proc(test_lum_mirror, 0, 0);
  proc(test_lum_mirror, 0, 1);

  // Then
  auto iter = prior_grid.begin();
  BOOST_CHECK_CLOSE(5.0, (*iter), 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(4.0, (*iter), 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter), 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter), 1E-3);

  BOOST_CHECK_CLOSE(5.0, proc.getMaxPrior(), 1E-3);
}



BOOST_AUTO_TEST_CASE(operator_function_mag) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleGrid prior_grid{axes};
  PhzDataModel::DoubleGrid scale_factor_grid{axes};

  auto scale_iter = scale_factor_grid.begin();
  (*scale_iter) = 3631E8;
  ++scale_iter;
  (*scale_iter) = 4;
  ++scale_iter;
  (*scale_iter) = 3;
  ++scale_iter;
  (*scale_iter) = 2;
  ++scale_iter;
  (*scale_iter) = 1;
  ++scale_iter;
  (*scale_iter) = 0;

  PhzLuminosity::LuminosityPrior::LuminosityGroupdProcessor proc(prior_grid, scale_factor_grid, true);

  // When
  proc(test_lum_abs, 0, 0);

  // Then
  auto iter = prior_grid.begin();
  BOOST_CHECK_CLOSE(5, (*iter), 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter), 1E-3);

  BOOST_CHECK_CLOSE(5.0, proc.getMaxPrior(), 1E-3);
}

BOOST_AUTO_TEST_SUITE_END ()
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE (LuminosityGroupSampledProcessor)

BOOST_AUTO_TEST_CASE(constructor) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);


  PhzDataModel::DoubleListGrid posterior_grid{axes};
  for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
    for (size_t index = 0; index < 4; ++index) {
      (*iter).push_back(0.0);
    }
  }
  PhzDataModel::DoubleGrid scale_factor_grid{axes};
  PhzDataModel::DoubleGrid sigma_scale_factor_grid{axes};

  // When
  PhzLuminosity::LuminosityPrior::LuminosityGroupSampledProcessor proc(posterior_grid,
                                                                       scale_factor_grid,
                                                                       sigma_scale_factor_grid,
                                                                       true,
                                                                       2.0,
                                                                       3);

  // Then
  BOOST_CHECK_CLOSE(0.0, proc.getMaxPrior(), 1E-3);
}



BOOST_AUTO_TEST_CASE(operator_function) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleListGrid prior_grid{axes};
  for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
    for (size_t index = 0; index < 4; ++index) {
      (*iter).push_back(0.0);
    }
  }
  PhzDataModel::DoubleGrid scale_factor_grid{axes};
  PhzDataModel::DoubleGrid sigma_scale_factor_grid{axes};

  PhzLuminosity::LuminosityPrior::LuminosityGroupSampledProcessor proc(prior_grid,
                                                                       scale_factor_grid,
                                                                       sigma_scale_factor_grid,
                                                                       false,
                                                                       2.0,
                                                                       3);


  // When
  proc(test_lum, 0, 0);

  // Then
  auto iter = prior_grid.begin();
  BOOST_CHECK_CLOSE(1.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(1.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(1.0, (*iter)[2], 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[2], 1E-3);
  BOOST_CHECK_CLOSE(1.0, proc.getMaxPrior(), 1E-3);
}


BOOST_AUTO_TEST_CASE(operator_function_flux) {
  // Given
  std::vector<double> zs {0.0, 1.0, 2.0};
  std::vector<double> ebvs {0.0};
  std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
  std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
  PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  PhzDataModel::DoubleListGrid prior_grid{axes};
    for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
      for (size_t index = 0; index < 4; ++index) {
        (*iter).push_back(0.0);
      }
  }
  PhzDataModel::DoubleGrid scale_factor_grid{axes};

  auto scale_iter = scale_factor_grid.begin();
  (*scale_iter) = 5;
  ++scale_iter;
  (*scale_iter) = 4;
  ++scale_iter;
  (*scale_iter) = 3;
  ++scale_iter;
  (*scale_iter) = 2;
  ++scale_iter;
  (*scale_iter) = 1;
  ++scale_iter;
  (*scale_iter) = 0;

  PhzDataModel::DoubleGrid sigma_scale_factor_grid{axes};
  for (auto sig_scale_iter = sigma_scale_factor_grid.begin(); sig_scale_iter != sigma_scale_factor_grid.end(); ++sig_scale_iter) {
    (*sig_scale_iter) = 0.5;
  }

  PhzLuminosity::LuminosityPrior::LuminosityGroupSampledProcessor proc(prior_grid,
                                                                        scale_factor_grid,
                                                                        sigma_scale_factor_grid,
                                                                        false,
                                                                        2.0,
                                                                        3);

  // When
  proc(test_lum_mirror, 0, 0);
  proc(test_lum_mirror, 0, 1);

  // Then
  auto iter = prior_grid.begin();
  BOOST_CHECK_CLOSE(4.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(5.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(6.0, (*iter)[2], 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(3.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(4.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(5.0, (*iter)[2], 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[2], 1E-3);
  ++iter;
  BOOST_CHECK_CLOSE(0.0, (*iter)[0], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[1], 1E-3);
  BOOST_CHECK_CLOSE(0.0, (*iter)[2], 1E-3);

  BOOST_CHECK_CLOSE(6.0, proc.getMaxPrior(), 1E-3);
}

BOOST_AUTO_TEST_CASE(operator_function_mag) {
  // Given
   std::vector<double> zs {0.0, 1.0, 2.0};
   std::vector<double> ebvs {0.0};
   std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
   std::vector<XYDataset::QualifiedName> seds {{"sed1"}, {"sed2"}};
   PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

   PhzDataModel::DoubleListGrid prior_grid{axes};
     for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
       for (size_t index = 0; index < 4; ++index) {
         (*iter).push_back(0.0);
       }
   }
   PhzDataModel::DoubleGrid scale_factor_grid{axes};

   auto scale_iter = scale_factor_grid.begin();
   (*scale_iter) = 3631E8;


   PhzDataModel::DoubleGrid sigma_scale_factor_grid{axes};
   auto sig_scale_iter = sigma_scale_factor_grid.begin();
   (*sig_scale_iter) = 0.5*3631E8;


   PhzLuminosity::LuminosityPrior::LuminosityGroupSampledProcessor proc(prior_grid,
                                                                         scale_factor_grid,
                                                                         sigma_scale_factor_grid,
                                                                         true,
                                                                         1,
                                                                         3);

   // When
   proc(test_lum_abs, 0, 0);

   // Then
   auto iter = prior_grid.begin();
   BOOST_CHECK_CLOSE(4.247425010840046, (*iter)[0], 1E-3);
   BOOST_CHECK_CLOSE(5.0, (*iter)[1], 1E-3);
   BOOST_CHECK_CLOSE(5.4402281476392025, (*iter)[2], 1E-3);
   ++iter;
   BOOST_CHECK_CLOSE(0.0, (*iter)[0], 1E-3);
   BOOST_CHECK_CLOSE(0.0, (*iter)[1], 1E-3);
   BOOST_CHECK_CLOSE(0.0, (*iter)[2], 1E-3);

   BOOST_CHECK_CLOSE(5.4402281476392025, proc.getMaxPrior(), 1E-3);

}

BOOST_AUTO_TEST_SUITE_END ()
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
    for (size_t index = 0; index < 4; ++index) {
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
    for (size_t index = 0; index < 4; ++index) {
      BOOST_CHECK((*iter)[index] == 0.);
    }
  }

}

BOOST_AUTO_TEST_CASE(applyEffectiveness) {
    std::vector<double> zs {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0};

    std::vector<double> ebvs {0.0};
    std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
    std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
    PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

    PhzDataModel::DoubleGrid posterior_grid{axes};

    PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
    PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
    grp_list["aa"] = sed_tp;


    std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                          std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};

    PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions), true, 0.5, 0.7);

    auto prior_grid = prior.createPriorGrid(posterior_grid);

    size_t index = 0;
    for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
      (*iter) = index;
      ++index;
    }

    std::vector<double> expected {1.8, 2.5, 3.2, 3.9, 4.6, 5.3, 6.0};

    // When
    prior.applyEffectiveness(prior_grid, 6.0);

    // Then

    index = 0;
    for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
      BOOST_CHECK_CLOSE((*iter), expected[index], 1E-3);
      ++index;
    }
}

BOOST_AUTO_TEST_CASE(applySampleEffectiveness) {
    std::vector<double> zs {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    std::vector<double> ebvs {0.0};
    std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
    std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
    PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

    PhzDataModel::DoubleListGrid posterior_grid{axes};
    for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
       for (size_t index = 0; index < 2; ++index) {
         (*iter).push_back(1.1);
       }
    }

    PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
    PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
    grp_list["aa"] = sed_tp;


    std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                          std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};

    PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions), true, 0.5, 0.7);

    auto prior_grid = prior.createListPriorGrid(posterior_grid);

    size_t index = 0;
    for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
      for (size_t ind = 0; ind < 2; ++ind) {
              (*iter)[ind] = index + ind*0.5;
       }
      ++index;
    }

    std::vector<double> expected_0 {1.8, 2.5, 3.2, 3.9, 4.6, 5.3};
    std::vector<double> expected_1 {2.15, 2.85, 3.55, 4.25, 4.95, 5.65};

    // When
    prior.applySampleEffectiveness(prior_grid, 6.0);

    // Then

    index = 0;
    for (auto iter = prior_grid.begin(); iter != prior_grid.end(); ++iter) {
      BOOST_CHECK_CLOSE((*iter)[0], expected_0[index], 1E-3);
      BOOST_CHECK_CLOSE((*iter)[1], expected_1[index], 1E-3);
      ++index;
    }
}


BOOST_AUTO_TEST_CASE(applyPrior) {
    std::vector<double> zs {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    std::vector<double> ebvs {0.0};
    std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
    std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
    PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

    PhzDataModel::DoubleGrid posterior_grid{axes};

    PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
    PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
    grp_list["aa"] = sed_tp;


    std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                          std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};

    PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions), true, 0.5, 0.7);

    auto prior_grid = prior.createPriorGrid(posterior_grid);

    auto iter_post = posterior_grid.begin();
    (*iter_post) = 0.0;
    ++iter_post;
    (*iter_post) = 0.0;
    ++iter_post;
    (*iter_post) = 0.0;
    ++iter_post;
    (*iter_post) = 1.0;
    ++iter_post;
    (*iter_post) = 1.0;
    ++iter_post;
    (*iter_post) = 1.0;

    auto iter_prior = prior_grid.begin();
    (*iter_prior) = 0.0;
    ++iter_prior;
    (*iter_prior) = 1.0;
    ++iter_prior;
    (*iter_prior) = 2.0;
    ++iter_prior;
    (*iter_prior) = 0.0;
    ++iter_prior;
    (*iter_prior) = 1.0;
    ++iter_prior;
    (*iter_prior) = 2.0;

    double min = std::numeric_limits<double>::lowest();
    std::vector<double> expected {min, 0.0, 0.6931471805599453, min, 1.0, 1.6931471805599453};

    // When
    prior.applyPrior(prior_grid, posterior_grid);

    // Then
    size_t index = 0;
    for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
      BOOST_CHECK_CLOSE((*iter), expected[index], 1E-3);
      ++index;
    }
}

BOOST_AUTO_TEST_CASE(applySamplePrior) {
    std::vector<double> zs {0.0, 1.0, 2.0};

    std::vector<double> ebvs {0.0};
    std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
    std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
    PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);


    PhzDataModel::DoubleListGrid posterior_grid{axes};
       for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
          for (size_t index = 0; index < 2; ++index) {
            (*iter).push_back(index);
          }
       }

    PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
    PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
    grp_list["aa"] = sed_tp;


    std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                          std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};

    PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions), true, 0.5, 0.7);

    auto prior_grid = prior.createListPriorGrid(posterior_grid);

    auto iter_prior = prior_grid.begin();
    (*iter_prior)[0] = 0.0;
    (*iter_prior)[1] = 0.0;
    ++iter_prior;
    (*iter_prior)[0] = 1.0;
    (*iter_prior)[1] = 1.0;
    ++iter_prior;
    (*iter_prior)[0] = 2.0;
    (*iter_prior)[1] = 2.0;
    ++iter_prior;

    double min = std::numeric_limits<double>::lowest();
    std::vector<double> expected_0 {min, 0.0, 0.6931471805599453};
    std::vector<double> expected_1 { min, 1.0, 1.6931471805599453};

    // When
    prior.applySamplePrior(prior_grid, posterior_grid);

    // Then
    size_t index = 0;
    for (auto iter = posterior_grid.begin(); iter != posterior_grid.end(); ++iter) {
      BOOST_CHECK_CLOSE((*iter)[0], expected_0[index], 1E-3);
      BOOST_CHECK_CLOSE((*iter)[1], expected_1[index], 1E-3);
      ++index;
    }
}
/*
class MocProc {
public:
  MocProc() {
    m_called = {};
  }

   void operator()(const std::function<double(double)>& , size_t sed_index, size_t z_index) {
     m_called.insert(sed_index*1000+z_index);
   }

   double getMaxPrior() {
     return 111.0;
   }

   std::set<int>& getCalled() {
     return m_called;
   }
private:
   std::set<int> m_called;
};

BOOST_AUTO_TEST_CASE(fillTheGrid) {
 // Given
 std::vector<double> zs {0.0, 1.0, 2.0, 3.0};

 std::vector<double> ebvs {0.0};
 std::vector<XYDataset::QualifiedName> reddeing_curves {{"red_curve1"}};
 std::vector<XYDataset::QualifiedName> seds {{"sed1"}};
 PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

 PhzDataModel::DoubleGrid posterior_grid{axes};

 PhzDataModel::QualifiedNameGroupManager::set_type  sed_tp{{"sed1"}, {"sed2"}};
 PhzDataModel::QualifiedNameGroupManager::group_list_type grp_list{};
 grp_list["aa"] = sed_tp;

 std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                         std::unique_ptr<MathUtils::Function>>> luminosityFunctions {};

 PhzLuminosity::LuminosityFunctionValidityDomain domain("aa", 0.0, 3.0);
 std::unique_ptr<MathUtils::Function> f_2 {new MathUtils::FunctionAdapter(test_lum)};
 luminosityFunctions.push_back(std::make_pair(domain, std::move(f_2)));

 PhzLuminosity::LuminosityPrior prior(PhzDataModel::QualifiedNameGroupManager{grp_list}, std::move(luminosityFunctions), false, 0.5, 0.7);

 // When
 auto prior_grid  = prior.createPriorGrid(posterior_grid);
 auto proc = MocProc();
 auto& sed_axis = prior_grid.getAxis<PhzDataModel::ModelParameter::SED>();
 auto& z_axis = prior_grid.getAxis<PhzDataModel::ModelParameter::Z>();

 double max = prior.fillTheGrid(proc, sed_axis, z_axis);

 BOOST_CHECK_CLOSE(max, 111, 1E-3);
 for (size_t z_index = 0; z_index < z_axis.size(); ++z_index) {
   for (size_t sed_index = 0; sed_index < sed_axis.size(); ++sed_index) {
       BOOST_CHECK(proc.getCalled().count(sed_index*1000+z_index) == 1);
   }
 }

}*/
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


