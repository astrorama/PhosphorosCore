/**
 * @file LikelihoodLogarithmAlgorithm.cpp
 * @date November 30, 2014
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"

namespace Euclid {
namespace PhzLikelihood {

LikelihoodLogarithmAlgorithm::LikelihoodLogarithmAlgorithm(ScaleFactorCalc         scale_factor_calc,
                                                           LikelihoodLogarithmCalc likelihood_log_calc)
    : m_scale_factor_calc{std::move(scale_factor_calc)}, m_likelihood_log_calc{std::move(likelihood_log_calc)} {}

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid