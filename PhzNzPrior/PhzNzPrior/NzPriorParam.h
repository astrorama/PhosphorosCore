/*
 * @file PhzNzPrior/NzPriorParam.h
 * @date 2019-03-15
 * @author Florian dubath
 */

#ifndef PHZ_NZ_PRIOR_NZPRIORPARAM_H_
#define PHZ_NZ_PRIOR_NZPRIORPARAM_H_

#include <vector>
#include <iterator>

namespace Euclid {
namespace PhzNzPrior {


class NzPriorParam{
public:

  NzPriorParam(double z0t1, double kmt1, double alphat1, double kt1, double ft1, double cst1,
               double z0t2, double kmt2, double alphat2, double kt2, double ft2, double cst2,
               double z0t3, double kmt3, double alphat3, double cst3);

  static NzPriorParam defaultParam();

  double getZ0t(size_t one_based_group_number);
  double getKmt(size_t one_based_group_number);
  double getAlphat(size_t one_based_group_number);
  double getKt(size_t one_based_group_number);
  double getFt(size_t one_based_group_number);
  double getCst(size_t one_based_group_number);



private:
  std::vector<double> m_z0t{};
  std::vector<double> m_kmt{};
  std::vector<double> m_alphat{};
  std::vector<double> m_kt{};
  std::vector<double> m_ft{};
  std::vector<double> m_cst{};

};

}
}


#endif /* PHZ_NZ_PRIOR_NZPRIORPARAM_H_ */
