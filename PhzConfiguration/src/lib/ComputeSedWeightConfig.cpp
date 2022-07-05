/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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
 */

/**
 * @file lib/ComputeSedWeightConfig.cpp
 * @date 28/05/2020
 * @author Florian Dubath
 */

#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string SED_WEIGHT_OUTPUT{"SED-Weight-Output"};

ComputeSedWeightConfig::ComputeSedWeightConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
  declareDependency<PhotometryGridConfig>();
  declareDependency<SedProviderConfig>();
  declareDependency<FilterConfig>();
  declareDependency<FilterProviderConfig>();
}

auto ComputeSedWeightConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute SED Weight options",
           {

               {SED_WEIGHT_OUTPUT.c_str(), po::value<std::string>()->default_value("SedWeight.ascii"),
                "Path of the file into which output the SED weights. Relative path are relative to "
                "<AuxDataDir>/GenericPriors/SedWeight/"}

           }}};
}

void ComputeSedWeightConfig::initialize(const UserValues& args) {
  auto file_name = args.find(SED_WEIGHT_OUTPUT)->second.as<std::string>();

  if (file_name.find("/") == 0) {
    // Absolute path
    m_output_file = file_name;
  } else {
    // relative to
    boost::filesystem::path dir(getDependency<AuxDataDirConfig>().getAuxDataDir() / "GenericPriors" / "SedWeight");
    boost::filesystem::create_directory(dir);
    fs::path result = getDependency<AuxDataDirConfig>().getAuxDataDir() / "GenericPriors" / "SedWeight" / file_name;
    m_output_file   = result.string();
  }
}

const std::string& ComputeSedWeightConfig::getOutputFile() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getOutputFile() on a not initialized instance.";
  }
  return m_output_file;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
