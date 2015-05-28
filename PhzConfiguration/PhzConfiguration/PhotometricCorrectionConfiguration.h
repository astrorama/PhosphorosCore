/** 
 * @file PhotometricCorrectionConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H
#define	PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"
#include "PhzConfiguration/CatalogNameConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometricCorrectionConfiguration
 * 
 * @brief
 * Class to be used by executables which want to have as input the photometric
 * correction
 * 
 * @details
 * The parameter options available is :
 * - photometric-correction-file : filename and path of the correction file
 * - enable-photometric-correction : Use or not the photometric corrections (YES or NO)
 * 
 * If the photometric-correction-file is a relative path, it is relative to the
 * directory INTERMEDIATE_DIR/CATALOG_NAME. If it is missing completely it is
 * set to the default INTERMEDIATE_DIR/CATALOG_NAME/photometric_corrections.txt.
 * It provides a photometriccorrection map with all the corrections set to 1 for
 * all the filters defined as photometry filters of the input catalog with the
 * filter-mapping-file parameter. Otherwise if the photometric-correction-file
 * is defined the correction data will be read from this correction file
 * 
 * When the Photometric Correction is defined, this class will be modified
 * accordingly.
 * @throw
 * ElementException: Photometric Correction file does not exist
 */
class PhotometricCorrectionConfiguration : public virtual PhosphorosPathConfiguration,
                                           public virtual CatalogNameConfiguration {
  
public:
  
  /// Returns no options
  static boost::program_options::options_description getProgramOptions();
  
  /// Creates a new instance of PhotometricCorrectionConfiguration
  PhotometricCorrectionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~PhotometricCorrectionConfiguration()=default;
  
  /// Returns a map with the photometric corrections for the filters
  PhzDataModel::PhotometricCorrectionMap getPhotometricCorrectionMap();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H */

