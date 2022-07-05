/**
 * @file ScaleFactorCalcMockForMap.h
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#ifndef SCALEFACTORCALCMOCKFORMAP_H
#define SCALEFACTORCALCMOCKFORMAP_H

#include "ElementsKernel/EnableGMock.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

using namespace testing;

namespace Euclid {

class ScaleFactorCalcMockForMap {

private:
public:
  virtual ~ScaleFactorCalcMockForMap() = default;

  typedef SourceCatalog::Photometry::const_iterator phot_iter;

  MOCK_METHOD3(FCall, double(phot_iter source_begin, phot_iter source_end, phot_iter model_begin));

  // The following returns a lambda object, which can be copied or moved, to be
  // used when these actions are needed (the mock instance does not support them). Note
  // that this object is valid only as long as the mock object is not deleted.
  PhzPhotometricCorrection::CalculateScaleFactorMap::ScaleFactorCalc getFunctorObject() {
    return [=](SourceCatalog::Photometry::const_iterator source_begin,
               SourceCatalog::Photometry::const_iterator source_end,
               SourceCatalog::Photometry::const_iterator model_begin) {
      return this->FCall(source_begin, source_end, model_begin);
    };
  }
};

}  // end of namespace Euclid

#endif /* SCALEFACTORCALCMOCKFORMAP_H */
