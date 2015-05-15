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
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "FluxErrorPair_boost.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"

using namespace testing;

namespace Euclid {

class LikelihoodFunctionMock {

public:

  virtual ~LikelihoodFunctionMock() = default;

  typedef PhzDataModel::PhotometryGrid::const_iterator phot_iter;

  MOCK_METHOD2(FunctorCall, PhzLikelihood::LikelihoodGridFunctor::result_type*(
                          const SourceCatalog::Photometry& source_photometry,
                          const PhzDataModel::PhotometryGrid& phot_grid)
  );
  
  PhzLikelihood::LikelihoodGridFunctor::result_type operator()(
                          const SourceCatalog::Photometry& source_photometry,
                          const PhzDataModel::PhotometryGrid& phot_grid) {
    std::unique_ptr<PhzLikelihood::LikelihoodGridFunctor::result_type> ptr {FunctorCall(source_photometry, phot_grid)};
    return PhzLikelihood::LikelihoodGridFunctor::result_type {std::move(*ptr)};
  }

  void expectFunctorCall(const SourceCatalog::Photometry& source_photometry, const PhzDataModel::PhotometryGrid& phot_grid) {
    EXPECT_CALL(*this, FunctorCall(_, _)).With(AllOf(
            Args<0>(Truly([this, source_photometry](std::tuple<const SourceCatalog::Photometry&> args) {

                      auto& recieved_photometry = std::get<0>(args);
                      BOOST_CHECK_EQUAL(source_photometry.size(),recieved_photometry.size());
                      auto expected_iter= source_photometry.begin();
                      for(auto& recieved:recieved_photometry) {
                        BOOST_CHECK(Elements::isEqual((*expected_iter).flux,recieved.flux));
                        ++expected_iter;
                      }
                      return true;
                    })),
            Args<1>(Truly([&phot_grid](std::tuple<const PhzDataModel::PhotometryGrid&> args) {
                      auto& received_grid = std::get<0>(args);
                      BOOST_CHECK_EQUAL(phot_grid.size(),received_grid.size());
                      for (auto phot_iter=phot_grid.begin(), received_iter=received_grid.begin();
                           phot_iter!=phot_grid.end(); ++phot_iter, ++received_iter) {
                        for (auto flux_iter=(*phot_iter).begin(), rec_flux_iter=(*received_iter).begin();
                                flux_iter!=(*phot_iter).end(); ++flux_iter, ++rec_flux_iter) {
                          BOOST_CHECK(Elements::isEqual((*flux_iter).flux, (*rec_flux_iter).flux));
                        }
                      }
                      return true;
                    } ))
        )).WillOnce(Return(new PhzLikelihood::LikelihoodGridFunctor::result_type {
                                        PhzDataModel::LikelihoodGrid{{"",{}},{"",{}},{"",{}},{"",{}}},
                                        PhzDataModel::ScaleFactordGrid{{"",{}},{"",{}},{"",{}},{"",{}}},
                                        0.
                    }));
  }
};

}
 // end of namespace Euclid

#endif	/* LIKELIHOODFUNCTIONCMOCK_H */

