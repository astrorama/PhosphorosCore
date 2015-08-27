/**
 * @file SedAxisCorrection.h
 * @date August 26, 2015
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_SEDAXISCORRECTION_H
#define	PHZLIKELIHOOD_SEDAXISCORRECTION_H

#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzLuminosity/SedGroupManager.h"

namespace Euclid {
namespace PhzLikelihood {


class SedAxisCorrection {

public:

  SedAxisCorrection(PhzLuminosity::SedGroupManager group_manager);

  void operator()(PhzDataModel::LikelihoodGrid& likelihood_grid);

private:
  PhzLuminosity::SedGroupManager m_group_manager;

};

}
}

#endif	/* PHZLIKELIHOOD_SEDAXISCORRECTION_H */

