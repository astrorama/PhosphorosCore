/*
 * BuildTemplatesConfiguration.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: Nicolas Morisset
 */

#include <cstdio>
#include <fstream>
#include <iostream>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/BuildTemplatesConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

BuildTemplatesConfiguration::BuildTemplatesConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
         : ParameterSpaceConfiguration(options), FilterConfiguration(options), IgmConfiguration(options) {

  m_options = options;

  // Extract file option
  std::string filename = m_options["output-photometry-grid"].as<std::string>();

  // The purpose here is to make sure we are able to
  // write the binary file on the disk
  std::fstream test_fstream;
  test_fstream.open(filename, std::fstream::out | std::fstream::binary);
  if ((test_fstream.rdstate() & std::fstream::failbit) != 0) {
    throw Elements::Exception() <<" IO error, can not write any file there : %s "
                                << filename << "from option : binary-photometry-grid ";
  }
  test_fstream.close();
  // Remove file created
  if (std::remove(filename.c_str())) {
    logger.warn() << "Removing temporary file creation failed: \"" << filename << "\" !";
  }

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
    GridContainer::gridBinaryExport(out, grid);
    logger.info() << "Created template photometries in file " << filename;
  };
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid

