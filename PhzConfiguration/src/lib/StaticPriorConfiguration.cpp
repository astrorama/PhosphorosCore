/** 
 * @file StaticPriorConfiguration.cpp
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzConfiguration/StaticPriorConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

boost::program_options::options_description StaticPriorConfiguration::getProgramOptions() {
  return {};
}

void StaticPriorConfiguration::addStaticPrior(PhzLikelihood::CatalogHandler::StaticPriorFunction prior) {
  m_static_priors.emplace_back(std::move(prior));
}

std::vector<PhzLikelihood::CatalogHandler::StaticPriorFunction> StaticPriorConfiguration::getStaticPriors() {
  return m_static_priors;
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid