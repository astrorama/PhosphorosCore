/** 
 * @file IgmConfiguration.cpp
 * @date April 29, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"
#include "PhzConfiguration/IgmConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description IgmConfiguration::getProgramOptions() {
  po::options_description options {"IGM absorption options"};
  options.add_options()
  ("igm-absorption-type", po::value<std::string>(),
        "The type of IGM absorption to apply (one of OFF, MADAU)");
  return options;
}

PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction IgmConfiguration::getIgmAbsorptionFunction() {
  auto type = getIgmAbsorptionType();
  if (type == "OFF") {
    return PhzModeling::NoIgmFunctor{};
  }
  if (type == "MADAU") {
    return PhzModeling::MadauIgmFunctor{};
  }
  throw Elements::Exception() << "Unknown IGM absorption type \"" 
                    << m_options["igm-absorption-type"].as<std::string>() << "\"";
}

std::string IgmConfiguration::getIgmAbsorptionType() {
  if (!m_options["igm-absorption-type"].empty()) { 
    return m_options["igm-absorption-type"].as<std::string>();
  }
  throw Elements::Exception() << "Missing mandatory parameter igm-absorption-type";
}

}
}