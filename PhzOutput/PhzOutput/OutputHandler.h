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

namespace Euclid {
namespace PhzOutput {

class OutputHandler {
  
public:
  
  virtual ~OutputHandler() {}
  
  virtual void handleSourceOutput(const SourceCatalog::Source& source,
                                  PhzDataModel::PhotometryGrid::const_iterator best_model,
                                  const PhzDataModel::Pdf1D& pdf) = 0;
  
};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_OUTPUTHANDLER_H */

