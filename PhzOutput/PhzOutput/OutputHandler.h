/**
 * @file OutputHandler.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_OUTPUTHANDLER_H
#define	PHZOUTPUT_OUTPUTHANDLER_H

#include <map>
#include <string>
#include "GridContainer/GridContainer.h"
#include "SourceCatalog/Source.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/LikelihoodGrid.h"

namespace Euclid {
namespace PhzOutput {

class OutputHandler {

public:

  /**
   * Defines the type of the PHZ results as following:
   * - An iterator pointing to the model photometry which is the best match
   * - A grid representing the 1D PDF over the redshift
   * - A map containing the likelihood (before applying priors) for all parameter space regions
   * - A map containing the posterior for all parameter space regions
   * - A double with the value of the alpha scale factor used for the fitting
   * - A double with the likelihood value of the best fitted model
   * - A map containing the best chi square for each region
   */
  typedef std::tuple<PhzDataModel::PhotometryGrid::const_iterator,         //0
                     PhzDataModel::Pdf1D,                                  //1
                     std::map<std::string, PhzDataModel::LikelihoodGrid>,  //2
                     std::map<std::string, PhzDataModel::LikelihoodGrid>,  //3
                     double,                                               //4
                     double,                                               //5
                     std::map<std::string, double>> result_type;           //6

  virtual ~OutputHandler() {}

  virtual void handleSourceOutput(const SourceCatalog::Source& source,
                                  const result_type& results) = 0;

};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_OUTPUTHANDLER_H */

