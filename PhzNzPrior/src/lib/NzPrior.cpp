/**
 * @file src/lib/NzPrior.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */

#include "PhzNzPrior/NzPrior.h"

#include <iterator>
#include <cmath>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"



namespace Euclid {
namespace PhzNzPrior {



static Elements::Logging logger = Elements::Logging::getLogger("NzPrior");


NzPrior::NzPrior(
   const PhzDataModel::QualifiedNameGroupManager& sedGroupManager,
   const XYDataset::QualifiedName& i_filter_name,
   const NzPriorParam& prior_param,
   double effectiveness): m_sedGroupManager{sedGroupManager},
       m_i_filter_name{i_filter_name},
       m_prior_param{prior_param},
       m_effectiveness{effectiveness} {}


 double pt__m0(double ft, double kt, double m0) {
   return ft*std::exp(-kt * (m0 - 20));
 }

 double zmt(double z0t, double kmt, double m0) {
   return z0t + kmt*(m0 - 20);
 }

double NzPrior::computeP_T_z__m0(double m0, double z, const XYDataset::QualifiedName& sed) {
  if (z <= 0.0) {
    z = 1e-4;
  }
  const auto& group_name = m_sedGroupManager.findGroupContaining(sed).first;

  double res = 0;
  if (group_name == "T1") {

    double pt_m0_1 = pt__m0(m_prior_param.getFt(1), m_prior_param.getKt(1), m0);
    double zmt_1 = zmt(m_prior_param.getZ0t(1), m_prior_param.getKmt(1), m0);
    double alpt_1 = m_prior_param.getAlphat(1);
    double A_1 = m_prior_param.getCst(1) * std::pow(zmt_1, alpt_1+1) / alpt_1;

    res = (pt_m0_1/A_1)*std::pow(z, alpt_1)*std::exp(-std::pow(z/zmt_1, alpt_1));

  } else if (group_name == "T2") {
    double pt_m0_2 = pt__m0(m_prior_param.getFt(2), m_prior_param.getKt(2), m0);
    double zmt_2 = zmt(m_prior_param.getZ0t(2), m_prior_param.getKmt(2), m0);
    double alpt_2 = m_prior_param.getAlphat(2);
    double A_2 = m_prior_param.getCst(2) * std::pow(zmt_2, alpt_2+1) / alpt_2;

    res = (pt_m0_2/A_2)*std::pow(z, alpt_2)*std::exp(-std::pow(z/zmt_2, alpt_2));

  } else {
    double pt_m0_1 = pt__m0(m_prior_param.getFt(1), m_prior_param.getKt(1), m0);
    double pt_m0_2 = pt__m0(m_prior_param.getFt(2), m_prior_param.getKt(2), m0);
    double pt_m0_3 = 1 - pt_m0_1 - pt_m0_2;

    double zmt_3 = zmt(m_prior_param.getZ0t(3), m_prior_param.getKmt(3), m0);
    double alpt_3 = m_prior_param.getAlphat(3);
    double A_3 = m_prior_param.getCst(3) * std::pow(zmt_3, alpt_3+1) / alpt_3;

    res = (pt_m0_3/A_3)*std::pow(z, alpt_3)*std::exp(-std::pow(z/zmt_3, alpt_3));

  }

  return res;
}

void NzPrior::operator()(PhzDataModel::RegionResults& results) {
  // 1. get the I magnitude
  auto& photometry = results.get<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>();
  auto flux_ptr = photometry.get().find(m_i_filter_name.qualifiedName());
  if (flux_ptr == nullptr) {
    throw Elements::Exception() << "NzPrior: Missing filter:"
                                << m_i_filter_name.qualifiedName()
                                << " in the source Photometry";
  }

  // flux is in micro Jy the AB mag factor is then 3613E6
  double mag_Iab = -2.5*std::log10(flux_ptr->flux/3.631E9);

  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();

  PhzDataModel::DoubleGrid prior_grid {posterior_grid.getAxesTuple()};
  for (auto& cell : prior_grid) {
    cell = 1.;
  }

  for (auto grid_iter=prior_grid.begin(); grid_iter != prior_grid.end(); ++grid_iter) {
    double z = grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
    const auto& sed = grid_iter.axisValue<PhzDataModel::ModelParameter::SED>();

    if (mag_Iab < 20) {
      if (z > 1) {
        *grid_iter = 0;
      }
    } else {
      *grid_iter = computeP_T_z__m0(mag_Iab, z, sed);
    }
  }

  // Apply the effectiveness to the prior.
  for (auto& v : prior_grid) {
    v = (1 - m_effectiveness) + m_effectiveness * v;
  }

  double min_value = std::numeric_limits<double>::lowest();
  // Apply the prior to the likelihood
  for (auto l_it = posterior_grid.begin(), p_it = prior_grid.begin();
            l_it != posterior_grid.end();
            ++l_it, ++p_it) {
      double prior = std::log(*p_it);
      if (*p_it == 0) {
        *l_it = min_value;
      } else  if (std::isfinite(prior)) {
          *l_it += prior;
      }
  }

}

}
}
