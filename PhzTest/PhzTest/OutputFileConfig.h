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
 * @file PhzTest/OutputFileConfig.h
 * @date 2016/02/09
 * @author florian dubath
 */

#ifndef _PHZTEST_OUTPUTFILECONFIG_H
#define _PHZTEST_OUTPUTFILECONFIG_H

#include <string>
#include <fstream>
#include <memory>
#include <boost/program_options.hpp>
#include "Configuration/Configuration.h"

#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
namespace po = boost::program_options;
namespace fs = boost::filesystem;


namespace Euclid {
namespace PhzTest {



static const std::string OUTPUT_FILE {"output-file"};

/**
 * @class OutputFileConfig
 *
 * @brief
 */
class OutputFileConfig : public Configuration::Configuration {

public:

  OutputFileConfig(long manager_id): Configuration::Configuration(manager_id){
     declareDependency<PhzConfiguration::ResultsDirConfig>();
     declareDependency<PhzConfiguration::CatalogTypeConfig>();
  }

  /**
   * @brief Destructor
   */
  virtual ~OutputFileConfig() = default;

  auto getProgramOptions () -> std::map<std::string, OptionDescriptionList> override {
    return { {"Output file options", {
          { OUTPUT_FILE.c_str(), po::value<std::string>(),
            "The path of the output file"}
        }}};
  }

  void initialize(const UserValues& args) override{
      auto& result_dir = getDependency<PhzConfiguration::ResultsDirConfig>().getResultsDir();
      auto& catalog_dir = getDependency<PhzConfiguration::CatalogTypeConfig>().getCatalogType();

      m_filename = result_dir / catalog_dir / "AbsoluteMag" / "mag.dat";
      if (args.count(OUTPUT_FILE) > 0) {
         fs::path path = args.find(OUTPUT_FILE)->second.as<std::string>();
         if (path.is_absolute()) {
           m_filename = path;
         } else {
           m_filename = result_dir / catalog_dir / "AbsoluteMag" / path;
         }
      }


  }

  fs::path getOutputFile(){
    return m_filename;
  }


private:
  fs::path m_filename;



}; /* End of OutputFileConfig class */

} /* namespace PhzTest */
} /* namespace Euclid */


#endif
