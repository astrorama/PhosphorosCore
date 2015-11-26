/**
 * @file ScaleFactorFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H
#define	PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H

#include <cmath>
#include <limits>
#include <iostream>
#include "ElementsKernel/Real.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"

//
// NOTE TO DEVELOPERS
//
// This file is not split to declarations (.h) and definitions (.icpp) on
// purpose. Performance tests have shown that splitting the file results to a
// performance penalty of 5% of the overall Phosphoros execution. Please don"t
// split this file without investigating and fixing this issue.
//

namespace Euclid {
namespace PhzLikelihood {

namespace _Impl {


/// Class that adds to the scale factor numerator and denominator the contribution
/// of a single filter
class NormalFractionAdder {
  
public:
  
  /**
   * @brief
   * Adds to the scale factor numerator and denominator the contribution of a 
   * single filter
   * 
   * @details
   * The numerator and denominator are computed as:
   * \f[
   *   \begin{aligned}
   *     N &= N_p + \frac{F_mF_s}{E_s^2} \\
   *     D &= D_p + \frac{F_m^2}{E_s^2}
   *   \end{aligned}
   * \f]
   * where:
   * - \f$ N \f$ : is the new value of the numerator
   * - \f$ N_p \f$ : is the previous value of the numerator
   * - \f$ D \f$ : is the new value of the denominator
   * - \f$ D_p \f$ : is the previous value of the denominator
   * - \f$ F_m \f$ : is the flux of the model
   * - \f$ F_s \f$ : is the flux of the source
   * - \f$ E_s \f$ : is the error of the source flux
   * 
   * Note that all the flags of the source FluxErrorPair are ignored and the
   * same calculation is performed for all the cases.
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param numerator
   *    A reference to the previous numerator value, which will be updated to
   *    the new one
   * @param denominator
   *    A reference to the previous denominator value, which will be updated to
   *    the new one
   */
  void operator()(const SourceCatalog::FluxErrorPair& source,
                  const SourceCatalog::FluxErrorPair& model,
                  double& numerator, double& denominator) {
    double error_square = source.error * source.error;
    // If the source flux is negative, which prevents the scale factor getting
    // negative values (flipping the template). Physically this means that we do
    // not allow for unrealistic negative fluxes in the models (which would mean
    // models absorpting light).
    numerator += (source.flux > 0) ? ((model.flux * source.flux) / error_square) : 0;
    denominator += (model.flux * model.flux) / error_square;
  }
  
}; // end of class NormalFractionAdder


/// Class that adds to the scale factor numerator and denominator the contribution
/// of a single filter, which takes into consideration missing data flags
class MissingDataFractionAdder {
  
public:
  
  /**
   * @brief
   * Adds to the scale factor numerator and denominator the contribution of a 
   * single filter, taking into consideration missing data flags
   * 
   * @details
   * If the missing flag of the source photometry is set, this filter is ignored
   * and the numerator and denominator values are not modified. If the missing
   * flag is not set, the returned value is the same as the one returned by the
   * NormalFractionAdder class.
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param numerator
   *    A reference to the previous numerator value, which will be updated to
   *    the new one
   * @param denominator
   *    A reference to the previous denominator value, which will be updated to
   *    the new one
   */
  void operator()(const SourceCatalog::FluxErrorPair& source,
                  const SourceCatalog::FluxErrorPair& model,
                  double& numerator, double& denominator) {
    if (!source.missing_photometry_flag) {
      NormalFractionAdder{}(source, model, numerator, denominator);
    }
  }
  
}; // end of class MissingDataFractionAdder


/**
 * @class ScaleFactorNormal
 * 
 * @brief
 * Computes the scale factor of a model for optimizing the chi square result
 * 
 * @tparam Adder
 *    A functor type for adding to the nominator and the denominator the
 *    contribution of a single filter, used for enabling or disabling missing
 *    data functionality. It must get as parameters the following:
 *    - The source FluxErrorPair
 *    - The model FluxErrorPair
 *    - A reference to a double containing the previous numerator value, which
 *      will be updated with the new one
 *    - A reference to a double containing the previous denominator value, which
 *      will be updated with the new one
 *    (for implementations see NormalFractionAdder and MissingDataFractionAdder
 *     classes)
 */
template <typename Adder>
class ScaleFactorNormal {
  
public:
  
  /**
   * @brief
   * Computes the scale factor of a model for minimizing the chi square result
   * 
   * @details
   * The scale factor is computed as:
   * \f[
   *   \alpha = \frac{\sum_{i=1}^{n}N_i}{\sum_{i=1}^{n}D_i}
   * \f]
   * where the numerator and denominator of filter \f$i\f$ are computed using the
   * template parameter type Adder.
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
   *    The scale factor which minimizes the chi square result
   */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source, SourceIter source_end, ModelIter model) {
    double numerator {0.0};
    double denominator {0.0};
    for (; source!=source_end; ++source, ++model) {
      Adder{}(*source, *model, numerator, denominator);
    }
    return numerator / denominator;
  }
  
}; // end of class ScaleFactorNormal


/**
 * @class ScaleFactorUpperLimit
 * 
 * @brief
 * Computes the scale factor of a model for optimizing the chi square result,
 * which takes into consideration photometries marked as no detections.
 * 
 * @tparam Adder
 *    A functor type for adding to the nominator and the denominator the
 *    contribution of a single filter, used for enabling or disabling missing
 *    data functionality. It must get as parameters the following:
 *    - The source FluxErrorPair
 *    - The model FluxErrorPair
 *    - A reference to a double containing the previous numerator value, which
 *      will be updated with the new one
 *    - A reference to a double containing the previous denominator value, which
 *      will be updated with the new one
 *    (for implementations see NormalFractionAdder and MissingDataFractionAdder
 *     classes)
 * 
 * @tparam LikelihoodLogarithmFunc
 *    A functor type for computing the likelihood logarithm. The chi square is
 *    computed from this as \f$ \chi^2 = -2*\ln \mathcal{L} \f$. It must get as 
 *    parameters the following:
 *    - An iterator pointing to the first source photometry
 *    - An iterator pointing to the one after the last source photometry
 *    - An iterator pointing to the first model photometry
 *    - The factor to scale the model before computing the residual
 * 
 *    Note that this type must be in sync with the type of Adder used for the
 *    results to make sense.
 */
template <typename Adder, typename LikelihoodLogarithmFunc>
class ScaleFactorUpperLimit {
  
public:
  
  /**
   * @brief
   * Computes the scale factor of a model for minimizing the chi square result,
   * handling correctly photometries flagged as upper limits
   * 
   * @details
   * If no source photometry is flagged as upper limit, this method returns the
   * same result with the ScaleFactorNormal<Adder> class. If there is at least
   * one source photometry flagged as upper limit, the scale factor is computed
   * by minimizing the LikelihoodLogaritmFunc function.
   * 
   * Note that depending on the type of the Adder template parameter, the missing
   * data flag is respected or not.
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
   *    The scale factor which minimizes the chi square result
   */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source, SourceIter source_end, ModelIter model) {
    
    bool has_upper_limit=false;
    for (auto it=source; it!=source_end; ++it){
      has_upper_limit |= (*it).upper_limit_flag;
    }
    if (!has_upper_limit) {
      return ScaleFactorNormal<Adder>{}(source, source_end, model);
    }
    
    double acc = 1E-5;
    double der_step = 1E-4;
    double a = 0;
    double step = 10.;
    double step_factor = .5;
    bool dir_right = true;
    double chi_a = -2 * LikelihoodLogarithmFunc{}(source, source_end, model, a);
    
    while (step > acc || step < -acc) {
      a += step;
      if (a < 0) {
        a -= step;
        step *= step_factor;
        continue;
      }
      double next_chi_a = -2 * LikelihoodLogarithmFunc{}(source, source_end, model, a);
      if (next_chi_a > chi_a) {
        a -= step;
        step *= step_factor;
        continue;
      } else {
        chi_a = next_chi_a;
      }
      double chi_da = -2 * LikelihoodLogarithmFunc{}(source, source_end, model, a+der_step);
      double d_chi = chi_da - chi_a;
      if (Elements::isEqual(d_chi, 0.)) {
        a -= step/2.;
        break;
      }
      if ((dir_right && (d_chi>0)) || ((!dir_right) && (d_chi<0))) {
        dir_right = !dir_right;
        step *= -step_factor;
      }
    }
    if (a < 0) {
      a = 0;
    }
    return a;
    
  }
  
}; // end of class ScaleFactorUpperLimit

} // end of namespace _Impl

/// Functor for computing the scale factor which optimizes the chi square
using ScaleFactorFunctorSimple = _Impl::ScaleFactorNormal<_Impl::NormalFractionAdder>;

/// Functor for computing the scale factor which optimizes the chi square, with
/// support for missing data
using ScaleFactorFunctorMissingData = _Impl::ScaleFactorNormal<_Impl::MissingDataFractionAdder>;

/// Functor for computing the scale factor which optimizes the chi square, with
/// support for upper limit
using ScaleFactorFunctorUpperLimit = _Impl::ScaleFactorUpperLimit<_Impl::NormalFractionAdder, ChiSquareLikelihoodLogarithmUpperLimit>;

/// Functor for computing the scale factor which optimizes the chi square, with
/// support for upper limit and missing data
using ScaleFactorFunctorUpperLimitMissingData = _Impl::ScaleFactorUpperLimit<_Impl::MissingDataFractionAdder, ChiSquareLikelihoodLogarithmUpperLimitMissingData>;

/// Functor for computing the scale factor which optimizes the chi square, with
/// support for upper limit (faster less accurate)
using ScaleFactorFunctorUpperLimitFast = _Impl::ScaleFactorUpperLimit<_Impl::NormalFractionAdder, ChiSquareLikelihoodLogarithmUpperLimitFast>;

/// Functor for computing the scale factor which optimizes the chi square, with
/// support for upper limit and missing data (faster less accurate)
using ScaleFactorFunctorUpperLimitFastMissingData = _Impl::ScaleFactorUpperLimit<_Impl::MissingDataFractionAdder, ChiSquareLikelihoodLogarithmUpperLimitFastMissingData>;

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H */

