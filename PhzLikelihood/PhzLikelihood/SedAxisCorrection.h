/**
 * @file SedAxisCorrection.h
 * @date August 26, 2015
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_SEDAXISCORRECTION_H
#define	PHZLIKELIHOOD_SEDAXISCORRECTION_H

#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzLikelihood {


class SedAxisCorrection {

public:

  SedAxisCorrection(PhzDataModel::QualifiedNameGroupManager group_manager);

  void operator()(PhzDataModel::LikelihoodGrid& likelihood_grid);

private:
  PhzDataModel::QualifiedNameGroupManager m_group_manager;

};

}
}

#endif	/* PHZLIKELIHOOD_SEDAXISCORRECTION_H */

