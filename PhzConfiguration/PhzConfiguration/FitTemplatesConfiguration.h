/**
 * @file FitTemplatesConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_FITTEMPLATESCONFIGURATION_H
#define	PHZCONFIGURATION_FITTEMPLATESCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzOutput/OutputHandler.h"
#include "PhzLikelihood/CatalogHandler.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"
#include "PhzConfiguration/PriorConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FitTemplatesConfiguration
 *
 * @brief Class defining the input configuration of the FitTemplates executable
 *
 * @details
 * It defines all the configuration required for the following inputs:
 * - Photometric correction
 * - Photometry catalog
 * - Model photometry grid
 * and it adds the required configuration for the output of the executable.
 */
class FitTemplatesConfiguration : virtual public PriorConfiguration,
                                  public PhotometricCorrectionConfiguration,
                                  public PhotometryCatalogConfiguration,
                                  public PhotometryGridConfiguration {

public:

  /**
   * @brief
   * Returns the program options for the FitTemplates executable
   *
   * @details
   * The returned options are the ones defined by the PhotometricCorrectionConfiguration,
   * the PhotometryCatalogConfiguration  and the PhotometryGridConfiguration,
   * plus the parameters:
   * 
   * - output-catalog-file   : the filename of the file to export the PHZ catalog
   * - output-catalog-format : the format of the PHZ catalog (ASCII, FITS)
   * - output-pdf-file       : the filename of the FITS file for the PDFs storage
   * - marginalization-type  : the type of marginalization algorithm (SUM, MAX)
   * - output-likelihood-dir : the directory where the likelihood grids will be stored
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new FitTemplatesConfiguration instance, for the given options
   *
   * @param options
   *    A map with the options and their values
   */
  FitTemplatesConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~FitTemplatesConfiguration()=default;

  /**
   * @brief
   * Returns the PhzOutput::OutputHandler to be used for producing the output of
   * the FitTemplates executable
   *
   * @dtails
   * The output handler currently returned is of the type PhzOutput::BestModelCatalog
   * and it stores in an ASCII catalog the best fitted model information.
   *
   * @return
   * The handler to use for storing the output
   */
  std::unique_ptr<PhzOutput::OutputHandler> getOutputHandler();
  
  PhzLikelihood::CatalogHandler::MarginalizationFunction getMarginalizationFunc();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_FITTEMPLATESCONFIGURATION_H */

