/*
 * SourcePhzCalculatorMock.h
 *
 *  Created on: Jan 16, 2015
 *      Author: fdubath
 */

#ifndef SOURCEPHZCALCULATOR_MOCK_H_
#define SOURCEPHZCALCULATOR_MOCK_H_

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/EnableGMock.h"

#include "PhzLikelihood/SourcePhzFunctor.h"

using namespace testing;

namespace Euclid {
class SourcePhzCalculatorMock{
private:

public:

  virtual ~SourcePhzCalculatorMock() = default;

  SourcePhzCalculatorMock(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
     const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
     PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction):
       m_phot_corr_map{std::move(phot_corr_map)},
       m_phot_grid(model_grid_map.at("")){
        expectFunctorCall();


      }

  MOCK_METHOD1(FunctorCall,
      PhzDataModel::SourceResults*(const SourceCatalog::Photometry& source_phot));

  PhzDataModel::SourceResults operator()(const SourceCatalog::Photometry& source_phot, double fixed_z=-99){
    std::unique_ptr< PhzDataModel::SourceResults> res(FunctorCall(source_phot));
    return std::move(*res);
  }

  void expectFunctorCall() {
    PhzDataModel::SourceResults result {};
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>(m_phot_grid.begin());
    result.set<PhzDataModel::SourceResultType::Z_1D_PDF>(PhzDataModel::Pdf1DParam<PhzDataModel::ModelParameter::Z>{GridContainer::GridAxis<double>{"Axis",{}}});
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>(0);
    result.set<PhzDataModel::SourceResultType::BEST_MODEL_POSTERIOR_LOG>(0);
    EXPECT_CALL(*this, FunctorCall(_)).WillOnce(Return(
        new PhzDataModel::SourceResults {result}
    ));
  };



private:
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const PhzDataModel::PhotometryGrid& m_phot_grid;
};




}

#endif /* SOURCEPHZCALCULATOR_MOCK_H_ */
