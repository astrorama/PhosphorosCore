/**
 * @file PriorConfiguration.cpp
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#include <functional>
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzConfiguration/PriorConfiguration.h"
#include "PhzConfiguration/LuminosityPriorConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_PRIOR {"luminosity-prior"};

boost::program_options::options_description PriorConfiguration::getProgramOptions() {
  po::options_description options {"Prior options"};
   options.add_options()
    (LUMINOSITY_PRIOR.c_str(), po::value<std::string>(),
        "If added, turn Luminosity Prior on  (YES/NO, default: NO)");
   return merge(options)
               (LuminosityPriorConfiguration::getProgramOptions());

   return options;
}

void PriorConfiguration::addPrior(PhzLikelihood::CatalogHandler::PriorFunction prior) {
  m_priors.emplace_back(std::move(prior));
}

std::vector<PhzLikelihood::CatalogHandler::PriorFunction> PriorConfiguration::getPriors() {
  if (!parsed){
    if(m_options.count(LUMINOSITY_PRIOR)==1 && m_options[LUMINOSITY_PRIOR].as<std::string>().compare("YES")==0){
      LuminosityPriorConfiguration lum_prior_conf{m_options};
      addPrior(lum_prior_conf.getLuminosityPrior());
    }

    parsed=true;
  }

  return m_priors;
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
