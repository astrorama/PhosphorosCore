/** 
 * @file NumericalAxisPrior.h
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_NUMERICALAXISPRIOR_H
#define	PHZLIKELIHOOD_NUMERICALAXISPRIOR_H

#include "PhzDataModel/LikelihoodGrid.h"

namespace Euclid {
namespace PhzLikelihood {

int my_count = 0;

template<int I>
class NumericalAxisPrior {
  
public:
  
  void operator()(PhzDataModel::LikelihoodGrid& likelihood_grid);
  
};

}
}

#include "PhzLikelihood/_impl/NumericalAxisPrior.h"

#endif	/* PHZLIKELIHOOD_NUMERICALAXISPRIOR_H */

