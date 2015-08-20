/**
 * @file LuminosityFunctionConfiguration.h
 * @date August 20, 2015
 * @author dubathf
 */

#ifndef PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIGURATION_H
#define	PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIGURATION_H

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "PhzLuminosity/LuminosityFunctionSet.h"

namespace Euclid {
namespace PhzConfiguration {

class LuminosityFunctionConfiguration {

public:

  static boost::program_options::options_description getProgramOptions();

  LuminosityFunctionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

   virtual ~LuminosityFunctionConfiguration()=default;

  PhzLuminosity::LuminosityFunctionSet getLuminosityFunction();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;


};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIGURATION_H */

