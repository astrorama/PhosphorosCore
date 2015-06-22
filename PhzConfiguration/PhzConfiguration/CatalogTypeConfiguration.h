/** 
 * @file CatalogTypeConfiguration.h
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CATALOGTYPECONFIGURATION_H
#define	PHZCONFIGURATION_CATALOGTYPECONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogTypeConfiguration
 * 
 * @brief
 * This class defines the configuration options related with grouping the 
 * Phosphoros inputs and outputs to catalogs.
 */
class CatalogTypeConfiguration {
  
public:
  
  /**
   * @brief
   * Returns the program options defined by the CatalogTypeConfiguration
   *
   * @details
   * These options are:
   * - catalog-type : The catalog type under which the inputs and outputs are grouped
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * Creates a new CatalogTypeConfiguration instance
   * 
   * @param options A map containing the user options
   */
  CatalogTypeConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
          : m_options{options} { }
  
  virtual ~CatalogTypeConfiguration() = default;
  
  /**
   * @brief
   * Returns the catalog type to use for grouping the input and the outputs
   * 
   * @returns the catalog type
   * @throws Elements::Exception
   *    If the parameter catalog-type is not given
   * @throws Elements::Exception
   *    If the parameter catalog-type contain characters other than [0-9a-zA-Z_]
   * @throws Elements::Exception
   *    If the parameter catalog-type is the empty string
   */
  std::string getCatalogType();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CATALOGTYPECONFIGURATION_H */

