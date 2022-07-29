/*
 * SourcePhzCalculatorMock.h
 *
 *  Created on: Jan 16, 2015
 *      Author: fdubath
 */

#ifndef SOURCEPHZCALCULATOR_MOCK_H_
#define SOURCEPHZCALCULATOR_MOCK_H_

#include "ElementsKernel/EnableGMock.h"
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "PhzLikelihood/SingleGridPhzFunctor.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "SourceCatalog/Source.h"

using namespace testing;

namespace Euclid {
class SourcePhzCalculatorMock {
private:
public:
  virtual ~SourcePhzCalculatorMock() = default;

  SourcePhzCalculatorMock(PhzDataModel::PhotometricCorrectionMap phot_corr_map, PhzDataModel::AdjustErrorParamMap,
                          const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
                          PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction, double,
                          std::vector<PhzLikelihood::SourcePhzFunctor::PriorFunction>,
                          std::vector<PhzLikelihood::SingleGridPhzFunctor::MarginalizationFunction>,
                          std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>>)
      : m_phot_corr_map{std::move(phot_corr_map)}, m_phot_grid(model_grid_map.at("")) {
    expectFunctorCall();
  }

  MOCK_METHOD1(FunctorCall, PhzDataModel::SourceResults*(const SourceCatalog::Source& source));

  PhzDataModel::SourceResults operator()(const SourceCatalog::Source& source) {
    std::unique_ptr<PhzDataModel::SourceResults> res(FunctorCall(source));
    return std::move(*res);
  }

  void expectFunctorCall() {
    PhzDataModel::SourceResults result{};
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>(m_phot_grid.begin());
    result.set<PhzDataModel::SourceResultType::Z_1D_PDF>(
        PhzDataModel::Pdf1DParam<PhzDataModel::ModelParameter::Z>{GridContainer::GridAxis<double>{"Axis", {}}});
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>(0);
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_POSTERIOR_LOG>(0);
    EXPECT_CALL(*this, FunctorCall(_)).WillOnce(Return(new PhzDataModel::SourceResults{result}));
  };

private:
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const PhzDataModel::PhotometryGrid&    m_phot_grid;
};

}  // namespace Euclid

#endif /* SOURCEPHZCALCULATOR_MOCK_H_ */
