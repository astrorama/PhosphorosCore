/**
 * @file src/lib/NzPriorParam.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */

#include "PhzNzPrior/NzPriorParam.h"

namespace Euclid {
namespace PhzNzPrior {

NzPriorParam::NzPriorParam(double z0t1, double kmt1, double alphat1, double kt1, double ft1, double cst1, double z0t2,
                           double kmt2, double alphat2, double kt2, double ft2, double cst2, double z0t3, double kmt3,
                           double alphat3, double cst3)
    : m_z0t{z0t1, z0t2, z0t3}
    , m_kmt{kmt1, kmt2, kmt3}
    , m_alphat{alphat1, alphat2, alphat3}
    , m_kt{kt1, kt2}
    , m_ft{ft1, ft2}
    , m_cst{cst1, cst2, cst3} {}

NzPriorParam NzPriorParam::defaultParam() {
  return NzPriorParam(0.431, 0.091, 2.46, 0.4, 0.30, 0.8869, 0.390, 0.100, 1.81, 0.3, 0.35, 0.8891, 0.300, 0.150, 2.00,
                      0.8874);
}

}  // namespace PhzNzPrior
}  // namespace Euclid
