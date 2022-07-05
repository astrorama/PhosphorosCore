/**
 * @file SigmaScaleFactorFunctor.h
 * @date 24/02/2021
 * @author dubathf
 */

#ifndef PHZLIKELIHOOD_SIGMASCALEFACTORFUNCTOR_H
#define PHZLIKELIHOOD_SIGMASCALEFACTORFUNCTOR_H

#include "ElementsKernel/Real.h"
#include <cmath>
#include <iostream>
#include <limits>

namespace Euclid {
namespace PhzLikelihood {

class SigmaScaleFactorFunctor {

public:
  /**
   * @brief
   * Computes the sigma of the scale factor
   *
   * @details
   * The sigma of the scale factor is computed as:
   * \f[
   *   sig_a_i = ((f_err_i * sed_flux/(f_err_i^2)) / sum(sed_flux^2/(f_err_i^2)) )^2
   *   sig_a = sqrt(sum(sig_a_i))
   * \f]
   * which simplify to
   *  \f[
   *   sig_a = 1/sqrt(sum(sed_flux^2/(f_err_i^2)))
   * \f]
   *
   * @tparam SourceIter
   *    The type which iterates over the source photometries. It must iterate
   *    over SourceCatalog::FluxErrorPair objects.
   * @tparam ModelIter
   *    The type which iterates over the model photometries. It must iterate
   *    over SourceCatalog::FluxErrorPair objects.
   *
   * @param source
   *    An iterator pointing to the first source photometry
   * @param source_end
   *    An iterator pointing to the one after the last source photometry
   * @param model
   *    An iterator pointing to the first model photometry
   *
   * @returns
   *    The sigma of the scale
   */
  template <typename SourceIter, typename ModelIter>
  double operator()(SourceIter source, SourceIter source_end, ModelIter model) {
    double denominator{0.0};
    for (; source != source_end; ++source, ++model) {
      if (!(*source).missing_photometry_flag) {
        denominator += (*model).flux * (*model).flux / ((*source).error * (*source).error);
      }
    }
    return 1.0 / std::sqrt(denominator);
  }
};  // end of class SigmaScaleFactorFunctor

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#endif /* PHZLIKELIHOOD_SIGMASCALEFACTORFUNCTOR_H */
