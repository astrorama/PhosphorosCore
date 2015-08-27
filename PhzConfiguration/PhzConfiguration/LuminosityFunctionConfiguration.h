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
#include "PhzConfiguration/LuminosityTypeConfiguration.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class LuminosityFunctionConfiguration: public virtual LuminosityTypeConfiguration,
                                       public virtual PhosphorosPathConfiguration{

public:

  static boost::program_options::options_description getProgramOptions();

  LuminosityFunctionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : LuminosityTypeConfiguration(options), PhosphorosPathConfiguration(options), m_options{options}{};

   virtual ~LuminosityFunctionConfiguration()=default;

  PhzLuminosity::LuminosityFunctionSet getLuminosityFunction();

private:
  template<typename returntype>
  returntype getOptionWithCheck(std::string optionName){
    if (m_options.count(optionName) == 0) {
            throw Elements::Exception() << "Missing mandatory option " << optionName;
    }

    return m_options[optionName].as<returntype>();
  }

  std::map<std::string, boost::program_options::variable_value> m_options;


};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYFUNCTIONCONFIGURATION_H */

