/** 
 * @file PhosphorosPathConfiguration.h
 * @date May 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOSPHOROSPATHCONFIGURATION_H
#define	PHZCONFIGURATION_PHOSPHOROSPATHCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhosphorosPathConfiguration
 * 
 * @brief
 * This class defines the configuration options related with the top level
 * Phosphros directories
 * 
 * @details
 * The Phosphoros top level directories are the following:
 * - <b>Phosphoros Root Directory:</b>
 *   The top level directory of Phosphoros
 * - <b>Catalogs Directory:</b>
 *   Contains all the catalog files
 * - <b>Auxiliary Data Directory:</b>
 *   Contains auxiliary data (SEDs, Filters, etc)
 * - <b>Intermediate Products Directory:</b>
 *   Contains files produced by Phosphoros, which are not final PHZ products
 * - <b>Results Directory:</b>
 *   Contains the final PHZ products
 * 
 * This class only sets the paths of the above directories, according the
 * Phosphoros rules. It does not perform any checks on them (for existance,
 * write permissions, etc).
 */
class PhosphorosPathConfiguration {
  
public:
  
  /**
   * @brief
   * Returns the program options defined by the PhosphorosPathConfiguration
   *
   * @details
   * These options are:
   * - phosphoros-root           : The top level directory of Phosphoros
   * - catalogs-dir              : The directory containing the catalog files
   * - aux-data-dir              : The directory containing the auxiliary data
   * - intermediate-products-dir : The directory containing the intermediate products
   * - results-dir               : The directory containing the final PHZ results
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * Creates a new PhosphorosPathConfiguration instance
   * 
   * @param options A map containing the user options
   * @throws ElementsException
   *    If the phosphoros-root option is not given and the PHOSPHOROS_ROOT
   *    environment variable is not an absolute path
   */
  PhosphorosPathConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  virtual ~PhosphorosPathConfiguration() = default;
  
  /**
   * @brief
   * Returns the top level directory of Phosphoros
   * 
   * @details
   * This directory is controlled with the environment variable `PHOSPHOROS_ROOT`
   * and the program option `phosphoros-root`. The precendance is the following:
   * - If the `phosphoros-root` option is given, it is used. A relative path is
   *   relative to the current working directory
   * - If the `phosphoros-root` option is not given and the `PHOSPHOROS_ROOT`
   *   environment variable is set, the environment variable is used. It must be
   *   an absolute path.
   * - If none of the above is set the default path `~/Phosphoros` is used.
   * 
   * @returns The Phosphoros root directory
   */
  boost::filesystem::path getPhosphorosRootDir();
  
  /**
   * @brief
   * Returns the directory containing the catalog files
   * 
   * @details
   * This directory is controlled with the program option `catalogs-dir` as
   * fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/Catalogs` is used
   * 
   * @returns The catalogs directory
   */
  boost::filesystem::path getCatalogsDir();
  
  /**
   * @brief
   * Returns the directory containing the auxiliary data
   * 
   * @details
   * This directory is controlled with the program option `aux-data-dir` as
   * fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/AuxiliaryData` is used
   * 
   * @returns The auxiliary data directory
   */
  boost::filesystem::path getAuxDataDir();
  
  /**
   * @brief
   * Returns the directory containing the intermediate products
   * 
   * @details
   * This directory is controlled with the program option `intermediate-products-dir`
   * as fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/IntermediateProducts` is used
   * 
   * @returns The intermediate products directory
   */
  boost::filesystem::path getIntermediateDir();
  
  /**
   * @brief
   * Returns the directory containing the final PHZ results
   * 
   * @details
   * This directory is controlled with the program option `results-dir` as
   * fllowing:
   * - If an absolute path is given it is used as is
   * - If a relative path is used, it is relative to the current working directory
   * - If no path is given, the default path `PHOSPHOROS_ROOT/Results` is used
   * 
   * @returns The final PHZ results directory
   */
  boost::filesystem::path getResultsDir();
  
private:
  
  boost::filesystem::path m_phosphoros_root;
  boost::filesystem::path m_catalogs;
  boost::filesystem::path m_aux_data;
  boost::filesystem::path m_intermediate;
  boost::filesystem::path m_results;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOSPHOROSPATHCONFIGURATION_H */

