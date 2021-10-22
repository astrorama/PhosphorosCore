/**
 * @file PhzModeling/ApplyFilterFunctor.h
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_APPLYFILTERFUNCTOR_H
#define PHZMODELING_APPLYFILTERFUNCTOR_H

#include "MathUtils/function/Function.h"
#include <utility>

namespace Euclid {

namespace XYDataset {
class XYDataset;
}

namespace PhzModeling {

/**
 * @class PhzModeling::ApplyFilterFunctor
 * @brief
 * This functor is in charge of applying a filter to a model dataset.
 * @details
 * This functor takes the model dataset and multiply its components by
 * the provided filter
 */
class ApplyFilterFunctor {

public:
  /**
   * @brief Function Call Operator
   * @details
   * Apply the filter to the Model dataset. The output grid contains knots within the filter range,
   * with the first and last values always 0. The grid resolution is based on the minimum dλ for both
   * the model and the filter transmission (if it is an interpolated function!).
   * The resolution of either one may be highest at some particular point (i.e. an emission line). Additionally,
   * when using filter shifts, the shift may mis-align the filter and SED features, breaking the correlation between
   * the shift and the resulting photometry.
   *
   * @param model
   * An XYDataset representing the Model to be filtered.
   *
   * @param filter_range
   * A pair of double defining in which interval
   * the filter assumes non zero values.
   *
   * @param filter
   * A Function which takes a wavelength in input and returns
   * the filter transmission (defined in interval [0,1])
   *
   * @return
   * A XYDataset representing the filtered Model.
   */
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& model, const std::pair<double, double>& filter_range,
                                  const MathUtils::Function& filter) const;
};

}  // end of namespace PhzModeling
}  // end of namespace Euclid

#endif /* PHZMODELING_APPLYFILTERFUNCTOR_H */
