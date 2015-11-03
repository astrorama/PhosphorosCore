/**
 * @file BayesianMarginalizationFunctor.h
 * @date March 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H
#define	PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H

#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzLikelihood/GroupedAxisCorrection.h"

namespace Euclid {
namespace PhzLikelihood {

class BayesianMarginalizationFunctor {

public:
  
  using SedAxisCorrection = GroupedAxisCorrection<PhzDataModel::ModelParameter::SED>;

  BayesianMarginalizationFunctor(){};

  BayesianMarginalizationFunctor(std::shared_ptr<SedAxisCorrection> sed_correction_ptr);

  PhzDataModel::Pdf1D operator()(const PhzDataModel::LikelihoodGrid& likelihood_grid) const;
private:
  std::shared_ptr<SedAxisCorrection> m_sed_correction_ptr = nullptr;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_BAYESIANMARGINALIZATIONFUNCTOR_H */

