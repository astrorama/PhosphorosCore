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

  MeiksinIgmFunctor(bool m_ots, bool m_lls);
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
private:
  
  bool m_ots;
  bool m_lls;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MEIKSINIGMFUNCTOR_H */

