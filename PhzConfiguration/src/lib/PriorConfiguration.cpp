/**
 * @file PriorConfiguration.cpp
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#include <functional>
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzConfiguration/PriorConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

boost::program_options::options_description PriorConfiguration::getProgramOptions() {
   return {};
}

void PriorConfiguration::addPrior(PhzLikelihood::CatalogHandler::PriorFunction prior) {
  m_priors.emplace_back(std::move(prior));
}

std::vector<PhzLikelihood::CatalogHandler::PriorFunction> PriorConfiguration::getPriors() {
  return m_priors;
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
