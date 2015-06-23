/**
 * @file BestFitFunctionMock.h
 * @date January 5, 2015
 * @author Florian Dubath
 */

#ifndef BESTFITFUNCTIONMOCK_H
#define	BESTFITFUNCTIONMOCK_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "FluxErrorPair_boost.h"

using namespace testing;

namespace Euclid {

class BestFitFunctionMock {

private:

public:


  virtual ~BestFitFunctionMock() = default;

  typedef PhzDataModel::LikelihoodGrid::iterator likely_iter;

  MOCK_METHOD2(FunctorCall, likely_iter(likely_iter likelihood_begin, likely_iter likelihood_end));

  void expectFunctorCall() { //likely_iter likelihood_begin, likely_iter likelihood_end
    EXPECT_CALL(*this, FunctorCall(_, _)).WillOnce(ReturnArg<0>());
  }
  
  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  PhzLikelihood::SourcePhzFunctor::BestFitSearchFunction getFunctorObject() {
    return [=](PhzDataModel::LikelihoodGrid::iterator likelihood_begin,
               PhzDataModel::LikelihoodGrid::iterator likelihood_end) {
      return this->FunctorCall(likelihood_begin, likelihood_end);
    };
  }
  
};

}
 // end of namespace Euclid

#endif	/* BESTFITFUNCTIONMOCK_H */

