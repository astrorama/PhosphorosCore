/**
 * @file LuminositySedGroupConfiguration.h
 * @date August 20, 2015
 * @author dubathf
 */

#ifndef PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIGURATION_H
#define	PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIGURATION_H

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzConfiguration {

class LuminositySedGroupConfiguration {

public:

  static boost::program_options::options_description getProgramOptions();

  LuminositySedGroupConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

   virtual ~LuminositySedGroupConfiguration()=default;

   PhzDataModel::QualifiedNameGroupManager getLuminositySedGroupManager();


private:

  std::map<std::string, boost::program_options::variable_value> m_options;


};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYSEDGROUPCONFIGURATION_H */

