/** 
 * @file IgmConfiguration.h
 * @date April 29, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_IGMCONFIGURATION_H
#define	PHZCONFIGURATION_IGMCONFIGURATION_H

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "PhzModeling/PhotometryGridCreator.h"

namespace Euclid {
namespace PhzConfiguration {
  
class IgmConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  IgmConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};
                     
   virtual ~IgmConfiguration()=default;
  
  PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction getIgmAbsorptionFunction();
  
  std::string getIgmAbsorptionType();
                     
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_IGMCONFIGURATION_H */

