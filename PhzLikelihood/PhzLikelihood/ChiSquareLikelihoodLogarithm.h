/**
 * @file ChiSquareLikelihoodLogarithm.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_CHISQUARELIKELIHOODLOGARITHM_H
#define	PHZLIKELIHOOD_CHISQUARELIKELIHOODLOGARITHM_H

#include <cmath> // for std::log, std::erf
#include "ElementsKernel/Real.h" // for isNotEqual
#include "ElementsKernel/MathConstants.h" // for sqrt_of_halfpi, sqrt_of_two
#include "SourceCatalog/SourceAttributes/Photometry.h" // for FluxErrorPair

//
// NOTE TO DEVELOPERS
//
// This file is not split to declarations (.h) and definitions (.icpp) on
// purpose. Performance tests have shown that splitting the file results to a
// performance penalty of 5% of the overall Phosphoros execution. Please don't
// split this file without investigating and fixing this issue.
//

namespace Euclid {
namespace PhzLikelihood {

namespace _Impl {

/**
 * @class ChiSquareLikelihoodLogarithm_Impl
 * 
 * @brief
 * Functor for computing the natural logarithm of the likelihood of a source
 * with respect to a given model
 * 
 * @details
 * The way the residual is computed between the source and model values is
 * delegated to the template parameter ResidualCalculator. Note that at the end
 * of this file are defined aliases for different ResidualCalculator specializations
 * which cover the normal usages of this class in Phosphoros.
 * 
 * @tparam ResidualCalculator
 *    The functor which is used for computing the residuals. It must accept as
 *    arguments the source value, the model value and the scale factor as double
 *    values (in this order) and to return a double value representing the residual.
 */
template <typename ResidualCalculator>
class ChiSquareLikelihoodLogarithm_Impl {

public:
  
  /**
   * @brief
   * Returns the logarithm of the likelihood
   * 
   * @details
   * The likelihood logarithm is computed as
   * \f$ -\frac{1}{2} \sum_{i} \left( Res_i \right) \f$, where \f$Res_i\f$ is
   * the residual computed by calling the ResidualCalculator with arguments the
   * source value, the model value and the scale.
   * 
   * @tparam SourceIter
   *    The type of the source values iterator. It must be an iterator over the
   *    same type as the first argument of the ResidualCalculator
   * @tparam ModelIter
   *    The type of the model values iterator. It must be an iterator over the
   *    same type as the second argument of the ResidualCalculator
   * 
   * @param source
   *    An iterator pointing to the first source value
   * @param source_end
   *    An iterator pointing to the one after the last source iterator
   * @param model
   *    An iterator pointing to the first model value
   * @param scale
   *    The factor to scale the model before computing the residual
   * 
   * @return The logarithm of the likelihood
   */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source, SourceIter source_end,
                    ModelIter model, double scale) const {
    double sum {0.0};
    for (; source!=source_end; ++source, ++model) {
      sum += ResidualCalculator{}(*source, *model, scale);
    }
    return -.5 * sum;
  }

}; // end of class ChiSquareFunctor_Impl


/// Functor class which returns the residuals following the chi square
class ChiSquareNormal {
  
public:
  
  /**
   * @brief
   * Computes the chi square residual between the source and the model
   * 
   * @details
   * The chi square is computed as:
   * \f[
   *   Res = \left( \frac{\alpha F_m-F_s}{E_s} \right )^2
   * \f]
   * where:
   * - \f$ \alpha \f$ : is the scale factor
   * - \f$ F_m \f$ : is the flux of the model photometry
   * - \f$ F_s \f$ : is the flux of the source photometry
   * - \f$ E_s \f$ : is the error of the source photometry
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param scale
   *    The scale factor to multiply the model with
   * @return 
   *    The chi square residual
   */
  double operator() (const SourceCatalog::FluxErrorPair& source,
                     const SourceCatalog::FluxErrorPair& model,
                     double scale) const {
    double difference = scale * model.flux - source.flux;
    // If the source error is zero we set it to the minimum positive value represented
    // by double precision, to avoid dividing with zero
    double error_square = (source.error != 0)
                        ? (source.error * source.error) 
                        : std::numeric_limits<double>::min();
    return difference * difference / error_square;
  }
  
}; // end of class ChiSquareNormal


/// Functor class which returns the residuals following the chi square, with
/// support for upper limit. For details see arXiv:1210.0285 but note that the
/// paper has got the equations wrong.
class ChiSquareUpperLimit {
  
public:
  
  /**
   * @brief
   * Computes the chi square residual between the source and the model, taking
   * into account photometries flagged as upper limits
   * 
   * @details
   * If the source photometry is not flagged as upper limit, the residual is the
   * same with the one returned by the ChiSquareNormal functor. If it is, the
   * residual is computed as:
   * \f[
   *   Res = -2\ln\left\{ \frac{1}{2} \left[ 1 +
   *          erf\left( \frac{F_{lim}-\alpha F_m}{\sqrt{2}E_s} \right) \right] \right\}
   * \f]
   * where:
   * - \f$ \alpha \f$ : is the scale factor
   * - \f$ F_m \f$ : is the flux of the model photometry
   * - \f$ F_{lim} \f$ : is the upper limit value
   * - \f$ E_s \f$ : is the error of the source photometry
   * 
   * Theoretically, when the model values is well above the upper limit, the
   * error function in the above equation evaluates close to -1, which results
   * to a residual value close to infinity. This is theoretically correct, because
   * it means that the model has likelihood close to zero. Practically though,
   * this computation is not possible, because float representation limitations.
   * More precisely, the std::erf() cannot return values infinitely close to -1,
   * so above a value it will return exactly -1. This results to a call of the
   * std::log() function with argument 0. This theoretically results to minus
   * infinity, so it results to an arithmetic error.
   * 
   * To avoid all the above complication, when the computed error function
   * returns -1, this method returns -2*math.log(sys.float_info.min), which is
   * the value 1416.7928370645282. This value is the biggest number that is not
   * infinity and results to a likelihood equal to zero.
   * 
   * Note that the flux value of the given source FluxErrorPair is expected to
   * be set as the upper limit value.
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param scale
   *    The scale factor to multiply the model with
   * @return 
   *    The chi square residual
   */
  double operator() (const SourceCatalog::FluxErrorPair& source,
                     const SourceCatalog::FluxErrorPair& model,
                     double scale) const {
    if (source.upper_limit_flag) {
      double err_func = std::erf((source.flux - scale * model.flux) /
                                 (Elements::Units::sqrt_of_two * source.error));
      if (err_func <= -1.) {
        return -2.0 * std::log(std::numeric_limits<double>::min());
      } else {
        return -2.0 * std::log(0.5 * (1 + err_func));
      }
    } else {
      return ChiSquareNormal{}(source, model, scale);
    }
  }
  
}; // end of class ChiSquareUpperLimit


/// Functor class which returns the residuals following the chi square, with
/// support for upper limit in a fast (and less accurate) way.
class ChiSquareUpperLimitFast {
  
public:
  
  /**
   * @brief
   * Computes the chi square residual between the source and the model, taking
   * into account photometries flagged as upper limits in a fast way
   * 
   * @details
   * If the source photometry is not flagged as upper limit, the residual is the
   * same with the one returned by the ChiSquareNormal functor. If it is, the
   * residual is computed as following:
   * \f[
   *   Res = \left\{\begin{matrix}
   *   0 & \text{, if\space\space} \alpha*F_m\leq F_{lim} \\
   *   1416.7928370645282 &  \text{, if\space\space} \alpha*F_m>F_{lim}
   *   \end{matrix}\right.
   * \f]
   * where:
   * - \f$ \alpha \f$ : is the scale factor
   * - \f$ F_m \f$ : is the flux of the model photometry
   * - \f$ F_{lim} \f$ : is the upper limit value
   * 
   * The value 1416.7928370645282 is the result of -2*math.log(sys.float_info.min)
   * which is the biggest number that is not infinity and results to a likelihood 
   * equal to zero.
   * 
   * Note that the flux value of the given source FluxErrorPair is expected to
   * be set as the upper limit value and that the error is ignored.
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param scale
   *    The scale factor to multiply the model with
   * @return 
   *    The chi square residual
   */
  double operator() (const SourceCatalog::FluxErrorPair& source,
                     const SourceCatalog::FluxErrorPair& model,
                     double scale) const {
    if (source.upper_limit_flag) {
      if (scale * model.flux <= source.flux) {
        return 0.;
      } else {
        return -2.0 * std::log(std::numeric_limits<double>::min());
      }
    } else {
      return ChiSquareNormal{}(source, model, scale);
    }
  }
  
}; // end of class ChiSquareUpperLimit


/// Functor class which returns the residuals following the chi square, with
/// support for missing data
template <typename ChiSquareFunc>
class ChiSquareMissingData {
  
public:
  
  /**
   * @brief
   * Computes the chie square residual between the source and the model, taking
   * into account photometries flagged as missing data
   * 
   * @details
   * If a source photometry is flagged as missing, the returned value is always
   * zero. If it is not missing, the computation of the residual is delegated to
   * the ChiSquareFunc template parameter.
   * 
   * @param source
   *    The source photometry information
   * @param model
   *    The model photometry information
   * @param scale
   *    The scale factor to multiply the model with
   * @return 
   *    The chi square residual
   */
  double operator() (const SourceCatalog::FluxErrorPair& source,
                     const SourceCatalog::FluxErrorPair& model,
                     double scale) const {
    if (source.missing_photometry_flag) {
      return 0;
    } else {
      return ChiSquareFunc{}(source, model, scale);
    }
  }
  
};

} // end of namesapce _Impl

/// Functor for computing the likelihood logarithm using the chi square
using ChiSquareLikelihoodLogarithmSimple = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareNormal>;

/// Functor for computing the likelihood logarithm using the chi square, with support
/// for missing data
using ChiSquareLikelihoodLogarithmMissingData = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareMissingData<_Impl::ChiSquareNormal>>;

/// Functor for computing the likelihood logarithm using the chi square, with support
/// for upper limit
using ChiSquareLikelihoodLogarithmUpperLimit = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareUpperLimit>;

/// Functor for computing the likelihood logarithm using the chi square, with support
/// for upper limit and missing data
using ChiSquareLikelihoodLogarithmUpperLimitMissingData = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareMissingData<_Impl::ChiSquareUpperLimit>>;

/// Functor for computing the likelihood logarithm using the chi square, with support
/// for upper limit (faster less accurate)
using ChiSquareLikelihoodLogarithmUpperLimitFast = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareUpperLimitFast>;

/// Functor for computing the likelihood logarithm using the chi square, with support
/// for upper limit (faster less accurate) and missing data
using ChiSquareLikelihoodLogarithmUpperLimitFastMissingData = _Impl::ChiSquareLikelihoodLogarithm_Impl<_Impl::ChiSquareMissingData<_Impl::ChiSquareUpperLimitFast>>;


} // end of namespce PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_CHISQUARELIKELIHOODLOGARITHM_H */

