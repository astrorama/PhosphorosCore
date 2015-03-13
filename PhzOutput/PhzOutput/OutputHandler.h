/** 
 * @file OutputHandler.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_OUTPUTHANDLER_H
#define	PHZOUTPUT_OUTPUTHANDLER_H

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
   * - A grid representing the multi dimensional likelihood
   */
  typedef std::tuple<PhzDataModel::PhotometryGrid::const_iterator,
                     PhzDataModel::Pdf1D, PhzDataModel::LikelihoodGrid> result_type;
  
  virtual ~OutputHandler() {}
  
  virtual void handleSourceOutput(const SourceCatalog::Source& source,
                                  const result_type& results) = 0;
  
};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_OUTPUTHANDLER_H */

