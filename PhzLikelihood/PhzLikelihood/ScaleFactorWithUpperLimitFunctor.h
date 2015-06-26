/**
 * @file ScaleFactorFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SCALEFACTORWITHUPPERLIMITFUNCTOR_H
#define	PHZLIKELIHOOD_SCALEFACTORWITHUPPERLIMITFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {
/**
 * @class Euclid::PhzLikelihood::ScaleFactorFunctor
 * @brief
 * This functor is in charge of computing the Scale Factor of the source. This factor
 * enters the computation of the Chi^2.
 * @details
 * The scale Factor is computed (analytically) as the value minimizing the Chi^2.
 * For a single Filter the Scale Factor is the ratio between the source flux and the model flux.
 * When multiple filter are available the Scale Factor is computed as the ratio between the
 * weighted sum of the source flux and the weighted sum of the model flux.
 * The applied weight being the model flux over the (source) error squared.
 * The model is assumed to be error free.
 */
template< typename InitialScaleFactorFunctor, typename ChiSquareFunctor>
class ScaleFactorWithUpperLimitFunctor {

public:
  ScaleFactorWithUpperLimitFunctor(double accuracy = 0.00000001, int loop_max = 50) :
    m_accuracy(accuracy),m_loop_max(loop_max){}

  /**
    * @brief Function Call Operator
    * @details
    * Compute the Scale Factor of the source with respect to the model.
    *
    * @param source_begin
    * An iterator over the Filters of the source exposing an object having "flux" and "error" fields.

    * @param source_end
    * End iterator for the Filters of the source.
    *
    * @param model_begin
    * An iterator over the Filters of the model exposing an object containing the model "flux".
    *
    * @return
    * The computed Scale Factor as a double.
    */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source_begin, SourceIter source_end,
                    ModelIter model_begin) const;

private:
  double m_accuracy;
  int m_loop_max;
  InitialScaleFactorFunctor m_initial_scale_factor_functor;
  ChiSquareFunctor m_chi_square_functor;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/ScaleFactorWithUpperLimitFunctor.icpp"

#endif	/* PHZLIKELIHOOD_SCALEFACTORWITHUPPERLIMITFUNCTOR_H */

