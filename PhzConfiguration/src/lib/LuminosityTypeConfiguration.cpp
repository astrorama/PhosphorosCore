/** 
 * @file LuminosityTypeConfiguration.cpp
 * @date August 20, 2015
 * @author dubathf
 */


#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminosityTypeConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_UNIT_TYPE {"luminosity-unit-type"};
static const std::string LUMINOSITY_WITH_REDDENING {"luminosity-with-reddening"};
static const std::string LUMINOSITY_FILTER {"luminosity-filter"};

po::options_description LuminosityTypeConfiguration::getProgramOptions() {
  po::options_description options {"Absolute luminosity computation options"};
  options.add_options()
  (LUMINOSITY_FILTER.c_str(), po::value<std::string>(),
      "The filter for which the luminosity is computed")
  (LUMINOSITY_UNIT_TYPE.c_str(), po::value<std::string>(),
        "The type of luminosity to be computed (one of MAGNITUDE/FLUX default: MAGNITUDE)")
  (LUMINOSITY_WITH_REDDENING.c_str(), po::value<std::string>(),
         "Determine if the absolute luminosity is computed with or without intrinsic reddening (YES/NO, default: YES)");
  return options;
}

bool LuminosityTypeConfiguration::luminosityInMagnitude() {
  if (m_options[LUMINOSITY_UNIT_TYPE].empty()) {
    return true;
  }

  std::string type = m_options[LUMINOSITY_UNIT_TYPE].as<std::string>();

  if (type == "MAGNITUDE") {
    return true;
  }
  if (type == "FLUX") {
    return false;
  }
  throw Elements::Exception() << "Unknown " << LUMINOSITY_UNIT_TYPE
      << " option \"" << type << "\"";
}

bool LuminosityTypeConfiguration::luminosityReddened(){
  if (m_options[LUMINOSITY_WITH_REDDENING].empty()) {
     return true;
   }

   std::string type = m_options[LUMINOSITY_WITH_REDDENING].as<std::string>();

   if (type == "YES") {
     return true;
   }
   if (type == "NO") {
     return false;
   }
   throw Elements::Exception() << "Unknown " << LUMINOSITY_WITH_REDDENING
       << " option \"" << type << "\"";
}


XYDataset::QualifiedName LuminosityTypeConfiguration::getLuminosityFilter(){
  if (m_options[LUMINOSITY_FILTER].empty()) {
    throw Elements::Exception() << "Missing mandatory option :" << LUMINOSITY_FILTER;
   }
  return {m_options[LUMINOSITY_FILTER].as<std::string>()};
}


}
}
