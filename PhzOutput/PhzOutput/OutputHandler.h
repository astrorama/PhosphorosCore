/**
 * @file OutputHandler.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_OUTPUTHANDLER_H
#define	PHZOUTPUT_OUTPUTHANDLER_H

#include "SourceCatalog/Source.h"
#include "PhzDataModel/SourceResults.h"

namespace Euclid {
namespace PhzOutput {

class OutputHandler {

public:

  virtual ~OutputHandler() {}

  virtual void handleSourceOutput(const SourceCatalog::Source& source,
                                  const PhzDataModel::SourceResults& results) = 0;

};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_OUTPUTHANDLER_H */

