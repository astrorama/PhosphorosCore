/**
 * @file LikelihoodScaleSampleLogarithmAlgorithm.cpp
 * @date 24/02/2021
 * @author dubathf
 */

#include "PhzLikelihood/LikelihoodScaleSampleLogarithmAlgorithm.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzLikelihood {

static Elements::Logging LikelihoodScaleSampleLogarithmAlgorithmlogger =
    Elements::Logging::getLogger("LikelihoodScaleSampleLogarithmAlgorithm");

LikelihoodScaleSampleLogarithmAlgorithm::LikelihoodScaleSampleLogarithmAlgorithm(
    LikelihoodLogarithmAlgorithm::ScaleFactorCalc scale_factor_calc, SigmaScaleFactorCalc sigma_scale_factor_calc,
    LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc likelihood_log_calc, size_t scale_sample_number,
    double scale_sample_range)
    : m_scale_factor_calc{std::move(scale_factor_calc)}
    , m_sigma_scale_factor_calc{std::move(sigma_scale_factor_calc)}
    , m_likelihood_log_calc{std::move(likelihood_log_calc)}
    , m_scale_sample_number{scale_sample_number}
    , m_scale_sample_range{scale_sample_range} {
  if (m_scale_sample_number < 3) {
    LikelihoodScaleSampleLogarithmAlgorithmlogger.warn() << "scale_sample_number parameter too small set to 3";
    m_scale_sample_number = 3;
  }

  if (m_scale_sample_number % 2 == 0) {
    LikelihoodScaleSampleLogarithmAlgorithmlogger.warn() << "scale_sample_number parameter is even: raised by 1";
    ++m_scale_sample_number;
  }
}

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid
