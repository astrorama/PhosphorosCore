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
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

BuildTemplatesConfiguration::BuildTemplatesConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
         : ParameterSpaceConfiguration(options), FilterConfiguration(options) {

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
      "The filename of the file to export in binary format the photometry grid")
  ("igm-absorption-type", po::value<std::string>(),
        "The type of IGM absorption to apply (one of OFF, MADAU)");

  options.add(ParameterSpaceConfiguration::getProgramOptions());
  options.add(FilterConfiguration::getProgramOptions());

  return options;
}


 BuildTemplatesConfiguration::OutputFunction BuildTemplatesConfiguration::getOutputFunction() {
  return [this](const PhzDataModel::PhotometryGrid& grid) {
    std::ofstream out {m_options["output-photometry-grid"].as<std::string>()};
    GridContainer::gridBinaryExport(out, grid);
  };
}

PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction BuildTemplatesConfiguration::getIgmAbsorptionFunction() {
  if (m_options["igm-absorption-type"].empty()) { 
    throw Elements::Exception() << "Missing mandatory parameter igm-absorption-type";
  }
  if (m_options["igm-absorption-type"].as<std::string>() == "OFF") {
    return PhzModeling::NoIgmFunctor{};
  }
  if (m_options["igm-absorption-type"].as<std::string>() == "MADAU") {
    return PhzModeling::MadauIgmFunctor{};
  }
  throw Elements::Exception() << "Unknown IGM absorption type \"" 
                    << m_options["igm-absorption-type"].as<std::string>() << "\"";
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid

