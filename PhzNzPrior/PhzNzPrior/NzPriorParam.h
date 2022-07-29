/*
 * @file PhzNzPrior/NzPriorParam.h
 * @date 2019-03-15
 * @author Florian dubath
 */

#ifndef PHZ_NZ_PRIOR_NZPRIORPARAM_H_
#define PHZ_NZ_PRIOR_NZPRIORPARAM_H_

#include <array>
#include <cassert>
#include <cstddef>

namespace Euclid {
namespace PhzNzPrior {

class NzPriorParam {
public:
  NzPriorParam(double z0t1, double kmt1, double alphat1, double kt1, double ft1, double cst1, double z0t2, double kmt2,
               double alphat2, double kt2, double ft2, double cst2, double z0t3, double kmt3, double alphat3,
               double cst3);

  static NzPriorParam defaultParam();

  double getZ0t(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_z0t.size());
    return m_z0t[one_based_group_number - 1];
  }

  double getKmt(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_kmt.size());
    return m_kmt[one_based_group_number - 1];
  }

  double getAlphat(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_alphat.size());
    return m_alphat[one_based_group_number - 1];
  }

  double getKt(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_kt.size());
    return m_kt[one_based_group_number - 1];
  }

  double getFt(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_ft.size());
    return m_ft[one_based_group_number - 1];
  }

  double getCst(size_t one_based_group_number) const {
    assert(one_based_group_number > 0 && one_based_group_number <= m_cst.size());
    return m_cst[one_based_group_number - 1];
  }

private:
  std::array<double, 3> m_z0t;
  std::array<double, 3> m_kmt;
  std::array<double, 3> m_alphat;
  std::array<double, 2> m_kt;
  std::array<double, 2> m_ft;
  std::array<double, 3> m_cst;
};

}  // namespace PhzNzPrior
}  // namespace Euclid

#endif /* PHZ_NZ_PRIOR_NZPRIORPARAM_H_ */
