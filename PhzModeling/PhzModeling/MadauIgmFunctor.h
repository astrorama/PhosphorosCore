/** 
 * @file MadauNewIgmFunctor.h
 * @date April 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_MADAUIGMFUNCTOR_H
#define	PHZMODELING_MADAUIGMFUNCTOR_H

#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
class MadauIgmFunctor {
  
public:
  
  MadauIgmFunctor(bool fast, bool metals);
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
private:
  
  bool m_fast;
  bool m_metals;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MADAUIGMFUNCTOR_H */

