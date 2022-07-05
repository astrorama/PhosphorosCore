/**
 * @file LikelihoodScaleSampleLogarithmAlgorithm.h
 * @date 24/02/2021
 * @author dubathf
 */

#ifndef PHZLIKELIHOOD_LIKELIHOODSCALESAMPLELOGARITHMALGORITHM_H
#define PHZLIKELIHOOD_LIKELIHOODSCALESAMPLELOGARITHMALGORITHM_H

#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include <functional>

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class LikelihoodScaleSampleLogarithmAlgorithm
 *
 * @brief Calculates the natural logarithm of the likelihood of a source over a
 * set of models and for a sampling of the scale factor
 *
 * @details
 * The LikelihoodScaleSampleLogarithmAlgorithm component is an STL-like algorithm
 * responsible for calculating the natural logarithm of the likelihood of a
 * source over a set of models and for a sampling of the scale factor,
 * based on the source and models photometries. The
 * assumption of the algorithm is that all the models provide photometries for
 * the same filters and in the same order. For the source the only restriction
 * is that it contains photometries for all the filters that the models have
 * (order doesn't matter and it might contain extra photometries).
 *
 *
 * The second way the algorithm can be parameterised is the likelihood logarithm
 * calculation method. For this is used the function LikelihoodLogarithmCalc, as
 * given by the constructor.
 */
class LikelihoodScaleSampleLogarithmAlgorithm {

public:
  /**
   * Definition of the function for calculating sigma on the factor with which a model
   * photometry must be scaled to best fit the source. It is a function which
   * gets the iterators over the source and the model photometries and it returns
   * the scale factor as a double. It makes the assumption that the iterators
   * will iterate over the photometries of the same filters and in the same
   * order.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin)>
      SigmaScaleFactorCalc;

  /**
   * Constructs a new instance of LikelihoodLogarithmAlgorithm.
   * @param scale_factor_calc The function to use for calculating the scale
   *        factor of each model
   * @param sigma_scale_factor_calc The function to use for calculating the sigma
   *        of the scale factor of each model
   * @param likelihood_log_calc The function to use for calculating the natural
   *        logarithm of the likelihood of a source to fit a model
   * @param scale_sample_number The number of sample in the scale factor dimension
   *        it has to be odd in order to have the central value sampled, so if even
   *        1 will be added to the value, 3 is the minimal value.
   * @param scale_sample_range sampling will be performed in
   *        [-ScaleSampleRange * sigma; ScaleSampleRange * sigma]
   */
  LikelihoodScaleSampleLogarithmAlgorithm(LikelihoodLogarithmAlgorithm::ScaleFactorCalc         scale_factor_calc,
                                          SigmaScaleFactorCalc                                  sigma_scale_factor_calc,
                                          LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc likelihood_log_calc,
                                          size_t scale_sample_number, double scale_sample_range);

  /**
   * Calculates the natural logarithm of the likelihood of a given source to fit
   * a set of models. The models are iterated by using the given iterator. They
   * are all assumed to provide photometry for the same filters and in the same
   * order. The source photometry is required to contain information for these
   * filters (but the order does not matter and there might be extra filters).
   * The output likelihood logarithm is stored using the likelihood_log_begin
   * iterator.
   *
   * @tparam ModelIter
   *    The type of the iterator over the model photometries. It
   *    must be an iterator over SourceCatalog::Photometry objects.
   * @tparam LikelihoodLogIter
   *    The type of the iterator which is used for returning
   *    the likelihood logarithm values. It must be an iterator over a collection of
   *    doubles, which has the same size as the number of models.
   * @tparam ScaleFactorIter
   *    The type of the iterator which is used for returning
   *    the scale factor values. It must be an iterator over a collection of
   *    doubles, which has the same size as the number of models.
   * @param source_photometry
   *    The photometry of the source to calculate the likelihood for
   * @param model_begin
   *    The iterator pointing at the first of the models
   * @param model_end
   *    The iterator pointing at one after the last of the models
   * @param likelihood_log_begin
   *    The iterator pointing at the first element of the collection which will
   *    be used as the likelihood logarithm output
   * @param scale_factor_begin
   *    The iterator pointing at the first element of the collection which will
   *    be used as the scale factor output
   * @param sigma_scale_factor_begin
   *    The iterator pointing at the first element of the collection which will
   *    be used as the sigma scale factor output
   * @param likelihood_log_sample_begin
   *    The iterator pointing at the first element of the collection which will
   *    be used as the likelihood sampling for the different value of the scale parameter output
   */
  template <typename ModelIter, typename LikelihoodLogIter, typename ScaleFactorIter, typename SigmaScaleFactorIter,
            typename LikelihoodSampleIter>
  void operator()(const SourceCatalog::Photometry& source_photometry, ModelIter model_begin, ModelIter model_end,
                  LikelihoodLogIter likelihood_log_begin, ScaleFactorIter scale_factor_begin,
                  SigmaScaleFactorIter sigma_scale_factor_begin,
                  LikelihoodSampleIter likelihood_log_sample_begin) const;

private:
  /**
   * @brief
   * The Scale Factor function.
   */

  LikelihoodLogarithmAlgorithm::ScaleFactorCalc m_scale_factor_calc;
  /**
   * @brief
   * The Sigma Scale Factor function.
   */

  SigmaScaleFactorCalc m_sigma_scale_factor_calc;
  /**
   * @brief
   * The Likelihood logarithm function.
   */
  LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc m_likelihood_log_calc;

  /**
   * @brief
   * The number of sample in the scale factor dimension.
   */
  size_t m_scale_sample_number;

  /**
   * @brief
   * Range for the scaling factor sampling in unit of sigma.
   */
  double m_scale_sample_range;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#include "PhzLikelihood/_impl/LikelihoodScaleSampleLogarithmAlgorithm.icpp"

#endif /* PHZLIKELIHOOD_LIKELIHOODSCALESAMPLELOGARITHMALGORITHM_H */
