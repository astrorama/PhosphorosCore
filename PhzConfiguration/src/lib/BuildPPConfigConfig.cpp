/**
 * @file src/lib/BuildPPConfigConfig.cpp
 * @date 2021-04-23
 * @author dubathf
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "PhzConfiguration/BuildPPConfigConfig.h"
#include "PhzConfiguration/SedConfig.h"


namespace po = boost::program_options;
using namespace Euclid::Configuration;


namespace Euclid {
namespace PhzConfiguration {


static const std::string PP{"physical-parameter"};
static const std::string OUTPUT_FILE{"output-file"};


BuildPPConfigConfig::BuildPPConfigConfig(long manager_id): Configuration(manager_id) {
  declareDependency<SedConfig>();

}

auto BuildPPConfigConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Build Physical Parameters Config Arguments", {
    {PP.c_str(), po::value<std::vector<std::string>>(), "List of the Physical Parameters to be used"},
    {OUTPUT_FILE.c_str(), po::value<std::string>(), "Name of the FITS file for outputing the config"}
  }}};
}

void BuildPPConfigConfig::initialize(const Euclid::Configuration::Configuration::UserValues &args) {
  if (args.count(OUTPUT_FILE)) {
    m_output_file = args.at(OUTPUT_FILE).as<std::string>();
  } else {
    throw Elements::Exception() << "Missing argument "+OUTPUT_FILE+".";
  }

  if (args.count(PP)) {
    m_param_list = args.at(PP).as<std::vector<std::string>>();
  }
}

const boost::filesystem::path& BuildPPConfigConfig::getOutputFilePath(void) const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
        throw Elements::Exception() << "Call to getOutputFilePath() on a not initialized instance.";
   }
   return m_output_file;
}

const std::vector<std::string>& BuildPPConfigConfig::getParamList(void) const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
        throw Elements::Exception() << "Call to getParamList() on a not initialized instance.";
   }
   return m_param_list;
}






}  // namespace PhzConfiguration
}  // namespace Euclid
