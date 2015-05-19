/*
 * BuildTemplatesConfiguration.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: Nicolas Morisset
 */

#include <cstdio>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/BuildTemplatesConfiguration.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzUtils/FileUtils.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

BuildTemplatesConfiguration::BuildTemplatesConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
         : ParameterSpaceConfiguration(options), FilterConfiguration(options), IgmConfiguration(options) {

  m_options = options;

  // Extract file option
  std::string filename = m_options["output-photometry-grid"].as<std::string>();

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(filename);

}

po::options_description BuildTemplatesConfiguration::getProgramOptions() {

  boost::program_options::options_description options {"Build Templates options"};

  options.add_options()
  ("output-photometry-grid", boost::program_options::value<std::string>(),
      "The filename of the file to export in binary format the photometry grid");

  options.add(ParameterSpaceConfiguration::getProgramOptions());
  options.add(FilterConfiguration::getProgramOptions());
  options.add(IgmConfiguration::getProgramOptions());

  return options;
}


BuildTemplatesConfiguration::OutputFunction BuildTemplatesConfiguration::getOutputFunction() {
  return [this](const PhzDataModel::PhotometryGrid& grid) {
    auto logger = Elements::Logging::getLogger("PhzOutput");
    auto filename = m_options["output-photometry-grid"].as<std::string>();
    std::ofstream out {filename};
    PhzDataModel::PhotometryGridInfo info{};
    info.axes = grid.getAxesTuple();
    info.igm_method = getIgmAbsorptionType();
    info.filter_names = getFilterList();
    boost::archive::binary_oarchive boa {out};
    boa << info;
    GridContainer::gridBinaryExport(out, grid);
    logger.info() << "Created template photometries in file " << filename;
  };
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid

