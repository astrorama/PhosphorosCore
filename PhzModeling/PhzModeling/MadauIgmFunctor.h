/** 
 * @file IgmAbsorptionFunctor.h
 * @date March 16, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_MADAUIGMFUNCTOR_H
#define	PHZMODELING_MADAUIGMFUNCTOR_H

#include <vector>
#include <map>
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
class MadauIgmFunctor {
  
public:
  
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed,
                                          double z) const;
  
private:
  
  mutable std::map<double, double> m_da_cache {};
  mutable std::map<double, double> m_db_cache {};
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MADAUIGMFUNCTOR_H */

