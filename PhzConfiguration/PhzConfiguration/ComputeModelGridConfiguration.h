/** 
 * @file ComputeModelGridConfiguration.h
 * @date November 4, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_COMPUTEMODELGRIDCONFIGURATION_H
#define	PHZCONFIGURATION_COMPUTEMODELGRIDCONFIGURATION_H

#include <functional>
#include <map>
#include <string>

#include <boost/program_options.hpp>

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzConfiguration/FilterConfiguration.h"
#include "PhzConfiguration/IgmConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ComputeModelGridConfiguration
 * @brief
 * This class defines the photometry grid parameter option used by the ComputeModelGrid
 * executable
 * @details
 * This class defines the Photometry Grid parameter option, \b binary-photometry-grid
 * and inherits from the options of the ParameterSpaceConfiguration, FilterConfiguration
 * and IgmConfiguration options. It also provides a function for writing a grid in a binary file.
 * The parameter available is :
 * - \b output-photometry-grid : string, output filename and path for storing the photometry grid data
 * Before writing data to the disk, the constructor checks that it is
 * possible to write on the disk at the location specified by the
 * binary-photometry-grid option and throws an exception if any.
 * @throw Element::Exception
 * - IO error, can not write any file there
 */

class ComputeModelGridConfiguration : public ParameterSpaceConfiguration,
                                      public FilterConfiguration,
                                      public IgmConfiguration {
  
public:
  
  typedef std::function<void(const PhzDataModel::PhotometryGrid&)> OutputFunction;
  
  /**
   * @brief
   * Get program options of the ComputeModelGridConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * It defines parameter options allowed for this class.
   * @details
   * The option is:
   *  - \b binary-photometry-grid : string, output filename for storing the grid data
   *  The constructor also makes sure we are able to write a file to the location
   *  defined by \b binary-photometry-grid
   * @param options
   * A map containing the options and their values.
   * @throw Element::Exception
   * - IO error, can not write any file there(to the location defined by \b binary-photometry-grid)
   *
   */
  ComputeModelGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

   /**
   * @brief destructor.
   */
   virtual ~ComputeModelGridConfiguration()=default;
  
  /**
   * @brief
   * This function provides a function with a PhzDataModel::PhotometryGrid
   * object as argument and it stores this object in a binary file
   *  with the filename provided by the binary-photometry-grid option.
   * @details
   * Before writing the object to the disk, the constructor checks that it is
   * possible to write data at the location specified by the \b
   * binary-photometry-grid option and throws an exception if any.
   * @throw Element::Exception
   * - IO error, can not write any file there
   * - If the igm-absorption-type parameter is missing
   * - If there are no filters set
   * @return
   * A function to be used for outputing the PhotometryGrid
   */
  OutputFunction getOutputFunction();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_COMPUTEMODELGRIDCONFIGURATION_H */

