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
      PhzLikelihood::SourcePhzFunctor::result_type*(const SourceCatalog::Photometry& source_phot));

  PhzLikelihood::SourcePhzFunctor::result_type operator()(const SourceCatalog::Photometry& source_phot){
    std::unique_ptr< PhzLikelihood::SourcePhzFunctor::result_type> res(FunctorCall(source_phot));
    return std::move(*res);
  }

  void expectFunctorCall() {
    std::map<std::string, PhzDataModel::LikelihoodGrid> likelihood_map {};
    likelihood_map.emplace(std::make_pair(std::string(""),
              PhzDataModel::LikelihoodGrid(PhzDataModel::createAxesTuple({},{},{},{}))));
    std::map<std::string, PhzDataModel::LikelihoodGrid> posterior_map {};
    posterior_map.emplace(std::make_pair(std::string(""),
              PhzDataModel::LikelihoodGrid(PhzDataModel::createAxesTuple({},{},{},{}))));
    PhzLikelihood::SourcePhzFunctor::result_type result {};
    result.setResult<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>(m_phot_grid.begin());
    result.setResult<PhzDataModel::SourceResultType::Z_1D_PDF>(PhzDataModel::Pdf1D{GridContainer::GridAxis<double>{"Axis",{}}});
    result.setResult<PhzDataModel::SourceResultType::LIKELIHOOD>(std::move(likelihood_map));
    result.setResult<PhzDataModel::SourceResultType::POSTERIOR>(std::move(posterior_map));
    result.setResult<PhzDataModel::SourceResultType::SCALE_FACTOR>(0);
    result.setResult<PhzDataModel::SourceResultType::BEST_MODEL_CHI_SQUARE>(0);
    result.setResult<PhzDataModel::SourceResultType::BEST_CHI_SQUARE_MAP>(std::map<std::string, double>{});
    EXPECT_CALL(*this, FunctorCall(_)).WillOnce(Return(
        new PhzLikelihood::SourcePhzFunctor::result_type {result}
    ));
  };



private:
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const PhzDataModel::PhotometryGrid& m_phot_grid;
};




}

#endif /* SOURCEPHZCALCULATOR_MOCK_H_ */
