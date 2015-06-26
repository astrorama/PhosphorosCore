/**
 * @file ScaleFactorWithUpperLimitFunctor.h
 * @date June 26, 2015
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_SCALEFACTORWITHUPPERLIMITFUNCTOR_H
#define	PHZLIKELIHOOD_SCALEFACTORWITHUPPERLIMITFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {
/**
 * @class Euclid::PhzLikelihood::ScaleFactorFunctor
 * @brief
 * This functor is in charge of computing the Scale Factor of the source.
 * It can handle the case of source with non detection (upper limit) in some
 * of the photometries. This factor enters the computation of the Chi^2.
 * @details
 * An approximate scale Factor is computed by the InitialScaleFactorFunctor,
 * then starting from this value, the algorithm do a numerical minimization
 * (parabolic interpolation, see https://www.uam.es/personal_pdi/ciencias/ppou/CNC/TEMA6/f10.pdf)
 * of the Chi^2 (which value is computed by the ChiSquareFunctor).
 * When the calculated Chi^2 is infinite (for model very unlikely) the initial
 * scale factor value is returned.
 */
template<typename InitialScaleFactorFunctor, typename ChiSquareFunctor>
class ScaleFactorWithUpperLimitFunctor {

public:
  /**
   * @brief constructor
   * @param accuracy
   * The calculation will stop if the difference between
   * the minimum value computed to the n-th step and the n+1 step is smaller than the accuracy.
   *
   * @param  loop_max
   * The calculation will stop if the expected accuracy is not reached after this
   * number of iteration. In this case a warning is outputed.
   */
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

