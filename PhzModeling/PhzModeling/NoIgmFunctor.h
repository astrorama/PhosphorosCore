/** 
 * @file NoIgmFunctor.h
 * @date March 17, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_NOIGMFUNCTOR_H
#define	PHZMODELING_NOIGMFUNCTOR_H

#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

class NoIgmFunctor {
  
public:
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_NOIGMFUNCTOR_H */

