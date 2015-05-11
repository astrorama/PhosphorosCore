/*
 * PhotometryGridConfguration.h
 *
 *  Created on: Dec 3, 2014
 *      Author: Nicolas Morisset
 */

#ifndef PHOTOMETRYGRIDCONFGURATION_H_
#define PHOTOMETRYGRIDCONFGURATION_H_

#include <map>
#include <string>

#include <boost/program_options.hpp>

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometryGridConfiguration
 * @brief
 * This class defines the Photometry Grid parameter option
 * @details
 * The parameter available is :
 * - \b photometry-grid-file, string, The path and filename of the grid file"
 * The getPhotometryGrid function loads a photometry grid in memory referred by the
 * photometry-grid-file parameter.
 * @throw Element::Exception
 * - Empty parameter option
 */

class PhotometryGridConfiguration {

public:

  /**
   * @brief
   * Get program options of the ReddeningConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
   static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * This class defines the photometry grid parameter options
   * @details
   * The options are:
   * photometry-grid-file : string, filename and path of the photometry grid file
   * @param options
   * A map containing the options and their values.
   *
   */
   PhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~PhotometryGridConfiguration()=default;


  /**
   * @brief getPhotometryGrid
   * This function loads in memory a photometry grid referred by \b photometry-grid-file
   * options as a PhzDataModel::PhotometryGrid object.
   * @details
   * @throw ElementException
   * Empty parameter option
   * @return
   * A PhzDataModel::PhotometryGrid object (the photometry grid)
   */
   PhzDataModel::PhotometryGrid getPhotometryGrid();
   
  /**
   * @brief
   * This function loads in memory only the photometry grid information from the
   * file given with the parameter photometry-grid-file, as a PhzDataModel::PhotometryGridInfo
   * object.
   * @throw ElementException
   * Empty parameter option
   * @return
   * A PhzDataModel::PhotometryGridInfo object (the photometry grid information)
   */
   PhzDataModel::PhotometryGridInfo getPhotometryGridInfo();

private:
  /// Map containing all the filter options and their values
  std::map<std::string, boost::program_options::variable_value> m_options;

};

}
}




#endif /* PHOTOMETRYGRIDCONFGURATION_H_ */
