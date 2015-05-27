/**
 * @file PhotometryCatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H

#include "PhzConfiguration/CatalogConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometryCatalogConfiguration
 *
 * @brief Configuration class to be used by executables which want to have as
 * input photometric catalogs
 *
 * @details
 * The catalog retrieved when using the getCatalog() method will contain sources
 * with the PhotometryAttribute, populated as described by the parameters (see
 * the getProgramOptions() documentation).
 */
class PhotometryCatalogConfiguration : virtual public CatalogConfiguration {

public:

  /**
   * @brief Returns the program options required for photometric catalog input
   *
   * @details
   * These options include all the options defined by the CatalogConfiguration
   * class, with the extra options
   * - missing-photometry-flag : The flux value to indicate missing photometry data
   * - filter-mapping-file : The file containing the catalog columns mapping
   * - exclude-filter : The photometries to ignore
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new PhotometryCatalogConfiguration instance, for the given
   * options
   *
   * @details
   * If the filter-mapping-file is a relative path, it is relative to the
   * `INTERMEDIATE_DIR/CATALOG_NAME` directory. If it is not given at all the
   * defalut`INTERMEDIATE_DIR/CATALOG_NAME/filter_mapping.txt` is used.
   * 
   * The filter-mapping-file must contain lines which contain three space
   * separated values, the filter name, the flux column name and the flux error
   * column name. Comments are allowed with the `#` character.
   *
   * @param options
   *    A map with the options and their values
   * @throws ElementsException
   *    if the filter-mapping-file does not exist
   * @throws ElementsException
   *    if the filter-mapping-file does not follow the format "filter flux_name error_name"
   * @throws ElementsException
   *    if any of the filters in the exclude-filter list is not in the mapping file
   * @throws ElementsException
   *    if the exclusion of filters leads to less than two photometries
   */
  PhotometryCatalogConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);
  
  /**
  * @brief destructor.
  */
  virtual ~PhotometryCatalogConfiguration()=default;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H */

