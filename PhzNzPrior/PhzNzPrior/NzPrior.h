/*
 * @file PhzNzPrior/NzPrior.h
 * @date 2019-03-15
 * @author Florian dubath
 */

#ifndef PHZ_NZ_PRIOR_NZPRIOR_H_
#define PHZ_NZ_PRIOR_NZPRIOR_H_

#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "XYDataset/QualifiedName.h"
#include "PhzNzPrior/NzPriorParam.h"

namespace Euclid {
namespace PhzNzPrior {


class NzPrior{
public:

  NzPrior(
    const PhzDataModel::QualifiedNameGroupManager& sedGroupManager,
    const XYDataset::QualifiedName& i_filter_name,
    const NzPriorParam& prior_param,
    double effectiveness = 1.0);

void operator()(PhzDataModel::RegionResults& results);

private:
  double computeP_T_z__m0(double m0, double z, const XYDataset::QualifiedName& sed);
  PhzDataModel::QualifiedNameGroupManager m_sedGroupManager;
  XYDataset::QualifiedName m_i_filter_name;
  NzPriorParam m_prior_param;
  double m_effectiveness = 1.0;
};

}
}


#endif /* PHZ_NZ_PRIOR_NZPRIOR_H_ */
