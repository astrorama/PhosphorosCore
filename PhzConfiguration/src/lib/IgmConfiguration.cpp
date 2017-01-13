/** 
 * @file IgmConfiguration.cpp
 * @date April 29, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/NoIgmFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "PhzConfiguration/IgmConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string IGM_ABSORPTION_TYPE {"igm-absorption-type"};

po::options_description IgmConfiguration::getProgramOptions() {
  po::options_description options {"IGM absorption options"};
  options.add_options()
  (IGM_ABSORPTION_TYPE.c_str(), po::value<std::string>(),
        "The type of IGM absorption to apply (one of OFF, MADAU, MEIKSIN, INOUE)");
  return options;
}

PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction IgmConfiguration::getIgmAbsorptionFunction() {
  auto type = getIgmAbsorptionType();
  if (m_options[IGM_ABSORPTION_TYPE].empty()) { 
    throw Elements::Exception() << "Missing mandatory parameter " << IGM_ABSORPTION_TYPE;
  }
  if (type == "OFF") {
    return PhzModeling::NoIgmFunctor{};
  }
  if (type == "MADAU") {
    return PhzModeling::MadauIgmFunctor{};
  }
  if (type == "MEIKSIN") {
    return PhzModeling::MeiksinIgmFunctor{};
  }
  if (type == "INOUE") {
    return PhzModeling::InoueIgmFunctor{};
  }
  throw Elements::Exception() << "Unknown " << IGM_ABSORPTION_TYPE << " option \"" << type << "\"";
}

std::string IgmConfiguration::getIgmAbsorptionType() {
  if (!m_options[IGM_ABSORPTION_TYPE].empty()) { 
    return m_options[IGM_ABSORPTION_TYPE].as<std::string>();
  }
  throw Elements::Exception() << "Missing mandatory parameter " << IGM_ABSORPTION_TYPE;
}

}
}