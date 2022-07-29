/**
 * @file tests/src/SharedPriorAdapter_test.cpp
 * @date Nov 4, 2015
 * @author Florian Dubath
 */
#include "ElementsKernel/EnableGMock.h"
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "XYDataset/QualifiedName.h"

#include "PhzLikelihood/SharedPriorAdapter.h"

using namespace testing;
using namespace Euclid;
using namespace Euclid::PhzDataModel;

struct SharedPriorAdaptor_Fixture {

  class PriorMock {
  public:
    void operator()(PhzDataModel::RegionResults& results) const {
      internal_call(results);
    }

    MOCK_CONST_METHOD1(internal_call, void(PhzDataModel::RegionResults& results));
  };

  XYDataset::QualifiedName luminosityFilterName{"group/FilterName"};

  std::vector<double>                           zs{0.0, 0.2, 0.6, 1.0};
  std::vector<double>                           ebvs{0.0, 0.1, 0.2};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve_1"}, {"sed/Curve_2"}, {"sed/Curve_3"}};

  Euclid::PhzDataModel::ModelAxesTuple parameter_space =
      Euclid::PhzDataModel::createAxesTuple(zs, ebvs, reddeing_curves, seds);

  // Shared pointer of filter name vector
  std::shared_ptr<std::vector<std::string>> filter_name_vector_ptr{
      new std::vector<std::string>{"group/FilterName1", "group/FilterName2"}};

  // Create the Grids
  RegionResults         results{};
  DoubleGrid&           scale_factor_grid = results.set<RegionResultType::SCALE_FACTOR_GRID>(parameter_space);
  PhotometryGrid        photometry_grid{parameter_space, *filter_name_vector_ptr};
  const PhotometryGrid& photometry_grid_ref =
      results.set<RegionResultType::MODEL_GRID_REFERENCE>(photometry_grid).get();
  DoubleGrid& posterior_grid = results.set<RegionResultType::POSTERIOR_LOG_GRID>(parameter_space);

  // photometry values vector
  std::vector<SourceCatalog::FluxErrorPair> photometry_vector{SourceCatalog::FluxErrorPair(1.0, 0.1),
                                                              SourceCatalog::FluxErrorPair(2.0, 0.3)};

  SourceCatalog::Photometry ref_photometry = SourceCatalog::Photometry{filter_name_vector_ptr, photometry_vector};

  SharedPriorAdaptor_Fixture() {

    double value = 1.7;
    for (auto photometry : photometry_grid) {
      std::vector<SourceCatalog::FluxErrorPair> local_photometry_vector{SourceCatalog::FluxErrorPair(value, 0.1),
                                                                        SourceCatalog::FluxErrorPair(0.1, 0.3)};
      photometry = SourceCatalog::Photometry{filter_name_vector_ptr, local_photometry_vector};
      value += 1.;
    }

    value = 1.3;
    for (auto& scale_factor : scale_factor_grid) {
      scale_factor = value;
      value += 1.;
    }

    value = 1.9;
    for (auto& likelihood : posterior_grid) {
      likelihood = value;
      value += 1.;
    }
  }
};

BOOST_AUTO_TEST_SUITE(SharedPriorAdapter_test)

BOOST_FIXTURE_TEST_CASE(functorCall, SharedPriorAdaptor_Fixture) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr, internal_call(_));

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  adaptor(results);
}

BOOST_FIXTURE_TEST_CASE(copy, SharedPriorAdaptor_Fixture) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr, internal_call(_)).Times(2);

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor_copied{adaptor};

  adaptor_copied(results);
  adaptor(results);
}

BOOST_FIXTURE_TEST_CASE(assigned, SharedPriorAdaptor_Fixture) {

  std::shared_ptr<PriorMock> prior_ptr{new PriorMock()};

  EXPECT_CALL(*prior_ptr, internal_call(_)).Times(2);

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor{prior_ptr};

  PhzLikelihood::SharedPriorAdapter<PriorMock> adaptor_assigned = adaptor;

  adaptor_assigned(results);
  adaptor(results);
}

BOOST_AUTO_TEST_SUITE_END()
