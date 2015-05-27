/*
 * SedConfiguration.h
 *
 *  Created on: Oct 17, 2014
 *      Author: Nicolas Morisset
 */

#ifndef SEDCONFIGURATION_H_
#define SEDCONFIGURATION_H_


#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/XYDatasetProvider.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class SedConfiguration
 * @brief
 * This class defines the Sed parameter options
 * @details
 * The parameters available are:
 * - \b sed-group     : string a sed group(only one) e.g. sed/MER
 * - \b sed-name      : string a sed name (only one) e.g. MER/vis
 * - \b sed-exclude   : string a sed name to be excluded (only one)
 * They all can be used multiple times
 * @throw Element::Exception
 * - Missing or unknown sed dataset provider options
 * - Empty sed list
 */

class SedConfiguration : public virtual PhosphorosPathConfiguration {

public:

  /**
   * @brief
   * Get program options of the SedConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * The sed configuration class defines parameters allowed for
   * the sed parameter options
   *
   * @details
   * The sed options are:
   * - sed-group     : vector of strings, goup name   e.g. sed/MER
   * - sed-name      : vector of strings, sed name e.g. sed/MER/vis
   * - sed-exclude   : vector of strings, sed name to be excluded
   * @param options
   * A map containing the options and their values
   *
   */
  SedConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~SedConfiguration()=default;

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @return
   * A unique boost::program_optionsinter of XYDatasetProvider type
   */
  std::unique_ptr<XYDataset::XYDatasetProvider> getSedDatasetProvider();

  /**
   * @brief
   * This function provides a sed list
   * @details
   * @throw Element::Exception
   * - Missing  <sed-root-path> or unknown sed dataset provider options
   * - Empty sed list
   * @return
   * A vector of QualifiedName type
   */
  std::vector<XYDataset::QualifiedName> getSedList();

private:
  /// Map containing all the sed options and their values
  std::map<std::string, boost::program_options::variable_value> m_options;

};

}
}


#endif /* SEDCONFIGURATION_H_ */
