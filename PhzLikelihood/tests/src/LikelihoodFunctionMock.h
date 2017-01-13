/**
 * @file LikelihoodFunctionMock.h
 * @date January 5, 2015
 * @author Florian Dubath
 */

#ifndef LIKELIHOODFUNCTIONCMOCK_H
#define	LIKELIHOODFUNCTIONCMOCK_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/DoubleGrid.h"
#include "FluxErrorPair_boost.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

using namespace testing;

namespace Euclid {

class LikelihoodFunctionMock {

public:

  virtual ~LikelihoodFunctionMock() = default;

  typedef PhzDataModel::PhotometryGrid::const_iterator phot_iter;

  MOCK_METHOD1(FunctorCall, void(PhzDataModel::RegionResults& results));
  
  void operator()(PhzDataModel::RegionResults& results) {
    FunctorCall(results);
  }

  void expectFunctorCall(const SourceCatalog::Photometry& source_photometry, const PhzDataModel::PhotometryGrid& phot_grid) {
    EXPECT_CALL(*this, FunctorCall(_)).WillOnce(Invoke(
            [source_photometry, &phot_grid](PhzDataModel::RegionResults& results) {
              using ResType = PhzDataModel::RegionResultType;
              
              // Check the source photometry
              auto& recieved_photometry = results.get<ResType::SOURCE_PHOTOMETRY_REFERENCE>().get();
              BOOST_CHECK_EQUAL(source_photometry.size(),recieved_photometry.size());
              auto expected_iter= source_photometry.begin();
              for(auto& recieved:recieved_photometry) {
                BOOST_CHECK(Elements::isEqual((*expected_iter).flux,recieved.flux));
                ++expected_iter;
              }
              
              // Check the model grid
              auto& received_grid = results.get<ResType::MODEL_GRID_REFERENCE>().get();
              BOOST_CHECK_EQUAL(phot_grid.size(),received_grid.size());
              for (auto phot_iter=phot_grid.begin(), received_iter=received_grid.begin();
                   phot_iter!=phot_grid.end(); ++phot_iter, ++received_iter) {
                for (auto flux_iter=(*phot_iter).begin(), rec_flux_iter=(*received_iter).begin();
                        flux_iter!=(*phot_iter).end(); ++flux_iter, ++rec_flux_iter) {
                  BOOST_CHECK(Elements::isEqual((*flux_iter).flux, (*rec_flux_iter).flux));
                }
              }
              
              // Add the likelihood and scale factor grids, full with zeros
              results.set<ResType::LIKELIHOOD_LOG_GRID>(phot_grid.getAxesTuple());
              results.set<ResType::SCALE_FACTOR_GRID>(phot_grid.getAxesTuple());
              
            }
    ));      
  }
  
  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction getFunctorObject() {
    return [=](PhzDataModel::RegionResults& results) {
      return this->operator ()(results);
    };
  }
  
};

}
 // end of namespace Euclid

#endif	/* LIKELIHOODFUNCTIONCMOCK_H */

