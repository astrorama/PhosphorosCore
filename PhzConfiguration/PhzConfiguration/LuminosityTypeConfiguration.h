/**
 * @file LuminosityTypeConfiguration.h
 * @date August 20, 2015
 * @author dubathf
 */

#ifndef PHZCONFIGURATION_LUMINOSITYTYPECONFIGURATION_H
#define	PHZCONFIGURATION_LUMINOSITYTYPECONFIGURATION_H

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzConfiguration {

class LuminosityTypeConfiguration {

public:

  static boost::program_options::options_description getProgramOptions();

  LuminosityTypeConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

   virtual ~LuminosityTypeConfiguration()=default;

  bool luminosityInMagnitude();

  bool luminosityReddened();

  XYDataset::QualifiedName getLuminosityFilter();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;


};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYTYPECONFIGURATION_H */

