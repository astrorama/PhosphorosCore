/** 
 * @file LuminosityFunctionConfiguration.cpp
 * @date August 20, 2015
 * @author dubathf
 */


#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"
#include "PhzConfiguration/LuminosityFunctionConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_FUNCTION_SED_GROUP {"luminosity-function-sed-group-"};
static const std::string LUMINOSITY_FUNCTION_MIN_Z {"luminosity-function-min-z-"};
static const std::string LUMINOSITY_FUNCTION_MAX_Z {"luminosity-function-max-z-"};

static const std::string LUMINOSITY_FUNCTION_CURVE_NAME {"luminosity-function-curve-"};

static const std::string LUMINOSITY_FUNCTION_SCHECHTER_ALPHA {"luminosity-function-schechter-alpha-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_M {"luminosity-function-schechter-m0-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_L {"luminosity-function-schechter-l0-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_PHI {"luminosity-function-schechter-phi0-"};



po::options_description LuminosityFunctionConfiguration::getProgramOptions() {
  po::options_description options {"Luminosity Function options, to be postfixed with the function id."};
  options.add_options()
  (LUMINOSITY_FUNCTION_SED_GROUP.c_str(), po::value<std::string>(),
      "The SED group the function is valid for (Mandatory for each Luminosity Function)")
  (LUMINOSITY_FUNCTION_MIN_Z.c_str(), po::value<std::string>(),
      "The lower bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  (LUMINOSITY_FUNCTION_MAX_Z.c_str(), po::value<std::string>(),
      "The upper bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  (LUMINOSITY_FUNCTION_CURVE_NAME.c_str(), po::value<std::string>(),
      "The sampling of the Curve defining the function (If omitted Schechter options are mandatories)")
  (LUMINOSITY_FUNCTION_SCHECHTER_ALPHA.c_str(), po::value<std::string>(),
      "The steepness of the Schechter luminosity function (Mandatory if the function-curve is skipped)")
  (LUMINOSITY_FUNCTION_SCHECHTER_M.c_str(), po::value<std::string>(),
      "The magnitude normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in magnitude)")
  (LUMINOSITY_FUNCTION_SCHECHTER_L.c_str(), po::value<std::string>(),
      "The flux normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in flux)")
  (LUMINOSITY_FUNCTION_SCHECHTER_PHI.c_str(), po::value<std::string>(),
      "The normalization value of the Schechter luminosity function (Mandatory if the function-curve is skipped)");
  return options;
}




PhzLuminosity::LuminosityFunctionSet LuminosityFunctionConfiguration::getLuminosityFunction(){
  // todo
  return PhzLuminosity::LuminosityFunctionSet{{}};
}


}
}
