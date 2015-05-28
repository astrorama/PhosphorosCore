/**
 * @file ComputeRedshiftsConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIGURATION_H
#define	PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIGURATION_H

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
 * @class ComputeRedshiftsConfiguration
 *
 * @brief Class defining the input configuration of the ComputeRedshifts executable
 *
 * @details
 * It defines all the configuration required for the following inputs:
 * - Photometric correction
 * - Photometry catalog
 * - Model photometry grid
 * and it adds the required configuration for the output of the executable.
 */
class ComputeRedshiftsConfiguration : public PhotometryCatalogConfiguration,
                                      public PhotometricCorrectionConfiguration,
                                      public PhotometryGridConfiguration,
                                      public PriorConfiguration {

public:

  /**
   * @brief
   * Returns the program options for the ComputeRedshifts executable
   *
   * @details
   * The returned options are the ones defined by the PhotometricCorrectionConfiguration,
   * the PhotometryCatalogConfiguration  and the PhotometryGridConfiguration,
   * plus the parameters:
   *
   * - phz-output-dir        : the phz output directory where to store output files
   * - output-catalog-format : the format of the PHZ catalog (ASCII, FITS)
   * - create-output-catalog : flag for storing the output catalog file(YES/NO, default: NO)
   * - create-output-pdf     : flag for storing the PDF file(YES/NO, default: NO)
   * - create-output-posteriors : flag for storing the posterior file(s)(YES/NO, default: NO)
   * - axes-collapse-type    : the method used for collapsing the axes
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new ComputeRedshiftsConfiguration instance, for the given options
   *
   * @param options
   *    A map with the options and their values
   */
  ComputeRedshiftsConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);

   /**
   * @brief destructor.
   */
   virtual ~ComputeRedshiftsConfiguration()=default;

  /**
   * @brief
   * Returns the PhzOutput::OutputHandler to be used for producing the output of
   * the ComputeRedshifts executable
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

#endif	/* PHZCONFIGURATION_COMPUTEREDSHIFTSCONFIGURATION_H */

