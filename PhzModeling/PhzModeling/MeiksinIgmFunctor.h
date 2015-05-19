/** 
 * @file MeiksinIgmFunctor.h
 * @date April 28, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_MEIKSINIGMFUNCTOR_H
#define	PHZMODELING_MEIKSINIGMFUNCTOR_H

#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
class MeiksinIgmFunctor {
  
public:
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MEIKSINIGMFUNCTOR_H */

