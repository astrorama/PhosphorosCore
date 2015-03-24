/** 
 * @file BayesianMarginalizationFunctor.h
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H

#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/LikelihoodGrid.h"

namespace Euclid {
namespace PhzLikelihood {

class BayesianMarginalizationFunctor {
  
public:
  
  PhzDataModel::Pdf1D operator()(const PhzDataModel::LikelihoodGrid& likelihood_grid) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H */

