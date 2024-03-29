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
 * @file src/lib/PhysicalParameter.cpp
 * @date 2021/04/22
 * @author Florian Dubath
 */

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/PhysicalParametersConfig.h"
#include "PhzDataModel/GridType.h"
#include "PhzDataModel/PPConfig.h"
#include "PhzOutput/PhzColumnHandlers/PhysicalParameter.h"
#include "Table/FitsReader.h"
#include <CCfits/CCfits>
#include <fstream>
#include <tuple>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string PP_CONFIG_FILE{"physical_parameter_config_file"};

static Elements::Logging logger = Elements::Logging::getLogger("PhysicalParametersConfig");

PhysicalParametersConfig::PhysicalParametersConfig(long manager_id) : Configuration(manager_id) {}

auto PhysicalParametersConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Physical parameters options",
           {{PP_CONFIG_FILE.c_str(), po::value<std::string>(),
             "Path to the FITS file containing the physical parameter configuration in a table with columns : "
             "PARAM_NAME, SED, A, B, C, D and Units. (C and D are optional) For each parameter a line must be present for each used SED and the param. "
             "value is p= A*L0 + B + C*LOG(D*L0)"}}}};
}

std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>>
PhysicalParametersConfig::readConfig(fs::path path) const {
  std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>> results{};

  std::ifstream sfile(path.generic_string());
  CCfits::FITS::setVerboseMode(true);

  // Check file exists
  if (sfile) {
    try {
      // Read first HDU
      auto table = Table::FitsReader{path.generic_string(), 1}.read();

      size_t col_number = table.getColumnInfo()->size();
      for (auto row : table) {
        std::string param_name = boost::get<std::string>(row[0]);
        std::string sed_name   = boost::get<std::string>(row[1]);
        double      param_A    = boost::get<double>(row[2]);
        double      param_B    = boost::get<double>(row[3]);
        double      param_C    = 0;
        double      param_D    = 0;
        std::string param_unit = "";
        if ( col_number==5){
        	param_unit = boost::get<std::string>(row[4]);
        } else if (col_number==7) {
            param_C    = boost::get<double>(row[4]);
            param_D    = boost::get<double>(row[5]);
        	param_unit = boost::get<std::string>(row[6]);
        }

        if (results.find(param_name) == results.end()) {
          results.insert(std::make_pair(param_name, std::map<std::string, PhzDataModel::PPConfig>()));
        }

        results.at(param_name).insert(std::make_pair(sed_name, PhzDataModel::PPConfig(param_A, param_B, param_C, param_D, param_unit)));
      }
    } catch (CCfits::FitsException& fits_except) {
      throw Elements::Exception() << "FitsException catched! File: " << path.generic_string();
    }
  }
  return results;
}

void PhysicalParametersConfig::initialize(const UserValues& args) {
  if (args.count(PP_CONFIG_FILE) > 0) {
    logger.debug() << "Configuring Physical parameters ";
    fs::path conf_file_path{args.find(PP_CONFIG_FILE)->second.as<std::string>()};

    m_param_config = readConfig(conf_file_path);
  }
}

const std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>>&
PhysicalParametersConfig::getParamConfig() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getParamConfig() on a not initialized instance.";
  }

  return m_param_config;
}

std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter>
PhysicalParametersConfig::getLikelihoodOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getLikelihoodOutputHandler() on a not initialized instance.";
  }
  return std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter>{
      new PhzOutput::ColumnHandlers::PhysicalParameter(PhzDataModel::GridType::LIKELIHOOD, m_param_config)};
}

std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter>
PhysicalParametersConfig::getPosteriorOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::INITIALIZED) {
    throw Elements::Exception() << "Call to getPosteriorOutputHandler() on a not initialized instance.";
  }

  return std::unique_ptr<PhzOutput::ColumnHandlers::PhysicalParameter>{
      new PhzOutput::ColumnHandlers::PhysicalParameter(PhzDataModel::GridType::POSTERIOR, m_param_config)};
}

}  // namespace PhzConfiguration
}  // namespace Euclid
