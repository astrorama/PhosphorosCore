/** 
 * @file ParameterSpaceConfiguration.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "ProgramOptionsHelper.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description ParameterSpaceConfiguration::getProgramOptions() {
  return merge(SedConfiguration::getProgramOptions())
              (ReddeningConfiguration::getProgramOptions())
              (RedshiftConfiguration::getProgramOptions());
}


}
}