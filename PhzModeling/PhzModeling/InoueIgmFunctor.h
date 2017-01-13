/** 
 * @file InoueIgmFunctor.h
 * @date May 6, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_INOUEIGMFUNCTOR_H
#define	PHZMODELING_INOUEIGMFUNCTOR_H

#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
class InoueIgmFunctor {
  
public:
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_INOUEIGMFUNCTOR_H */

