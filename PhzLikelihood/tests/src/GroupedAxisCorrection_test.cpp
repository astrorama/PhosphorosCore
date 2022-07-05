/**
 * @file PhzLikelihood/_impl/GroupedAxisCorrection_test.icpp
 * @date 11/03/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"

using namespace Euclid;
using namespace Euclid::PhzLikelihood;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(GroupedAxisCorrection_test)

//-----------------------------------------------------------------------------
// Checks that the functor call applies correctly the correction
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(functorCall) {

  // Given
  std::vector<double>                   z_values{0.0, 0.1, 0.2, 0.4, 0.5};
  std::vector<double>                   ebv_values{0.0, 0.01, 0.03, 0.04, 0.05};
  std::vector<XYDataset::QualifiedName> reddening_curves{{"Curve1"}, {"Curve2"}};
  std::vector<XYDataset::QualifiedName> seds{{"Sed1"}, {"Sed2"}, {"Sed3"}, {"Sed4"}, {"Sed5"}, {"Sed6"}};
  PhzDataModel::DoubleGrid likelihood_grid{PhzDataModel::createAxesTuple(z_values, ebv_values, reddening_curves, seds)};
  for (auto& cell : likelihood_grid) {
    cell = 1.;
  }
  PhzDataModel::QualifiedNameGroupManager group_manager{
      {{"Group1", {{"Sed1"}, {"Sed2"}}}, {"Group2", {{"Sed3"}, {"Sed4"}, {"Sed5"}, {"Sed6"}}}}};
  GroupedAxisCorrection<PhzDataModel::ModelParameter::SED> correction{group_manager};

  // When
  correction(likelihood_grid);

  // Then
  for (auto& group : group_manager.getManagedGroups()) {
    double expected = 1. / group.second.size();
    for (auto& sed : group.second) {
      for (auto iter = likelihood_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);
           iter != likelihood_grid.end(); ++iter) {
        BOOST_CHECK_EQUAL(*iter, expected);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Checks that the functor call throws exception when the axis contains a sed
// that is not in the groups
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(functorCall_exception) {

  // Given
  std::vector<double>                   z_values{0.0, 0.1, 0.2, 0.4, 0.5};
  std::vector<double>                   ebv_values{0.0, 0.01, 0.03, 0.04, 0.05};
  std::vector<XYDataset::QualifiedName> reddening_curves{{"Curve1"}, {"Curve2"}};
  std::vector<XYDataset::QualifiedName> seds{{"Sed1"}, {"Sed2"}, {"Sed3"}, {"Sed4"}, {"Sed5"}, {"Sed6"}};
  PhzDataModel::DoubleGrid likelihood_grid{PhzDataModel::createAxesTuple(z_values, ebv_values, reddening_curves, seds)};
  for (auto& cell : likelihood_grid) {
    cell = 1.;
  }
  PhzDataModel::QualifiedNameGroupManager group_manager{
      {{"Group1", {{"Sed1"}, {"Sed2"}}}, {"Group2", {{"Sed3"}, {"Sed4"}, {"Sed6"}}}}};
  GroupedAxisCorrection<PhzDataModel::ModelParameter::SED> correction{group_manager};

  // Then
  BOOST_CHECK_THROW(correction(likelihood_grid), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
