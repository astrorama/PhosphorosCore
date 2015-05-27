/** 
 * @file CatalogNameConfiguration.h
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CATALOGNAMECONFIGURATION_H
#define	PHZCONFIGURATION_CATALOGNAMECONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogNameConfiguration
 * 
 * @brief
 * This class defines the configuration options related with grouping the 
 * Phosphoros inputs and outputs to catalogs.
 */
class CatalogNameConfiguration {
  
public:
  
  /**
   * @brief
   * Returns the program options defined by the CatalogNameConfiguration
   *
   * @details
   * These options are:
   * - catalog-name : The catalog name under which the inputs and outputs are grouped
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * Creates a new CatalogNameConfiguration instance
   * 
   * @param options A map containing the user options
   */
  CatalogNameConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
          : m_options{options} { }
  
  virtual ~CatalogNameConfiguration() = default;
  
  /**
   * @brief
   * Returns the catalog name to use for grouping the input and the outputs
   * 
   * @returns the catalog name
   * @throws Elements::Exception
   *    If the parameter catalog-name is not given
   * @throws Elements::Exception
   *    If the parameter catalog-name contain characters other than [0-9a-zA-Z_]
   * @throws Elements::Exception
   *    If the parameter catalog-name is the empty string
   */
  std::string getCatalogName();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CATALOGNAMECONFIGURATION_H */

