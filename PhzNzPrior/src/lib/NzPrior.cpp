/**
 * @file src/lib/NzPrior.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */

#include "PhzNzPrior/NzPrior.h"

#include <cmath>
#include <iterator>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzNzPrior {

static Elements::Logging logger = Elements::Logging::getLogger("NzPrior");

static const std::string MISSING_FLUX_FOR_NZ_FLAG{"MISSING_FLUX_FOR_NZ_FLAG"};

NzPrior::NzPrior(const PhzDataModel::QualifiedNameGroupManager& sedGroupManager, const XYDataset::QualifiedName& i_filter_name,
                 const NzPriorParam& prior_param, double effectiveness)
    : m_sedGroupManager{sedGroupManager}
    , m_i_filter_name{i_filter_name}
    , m_prior_param{prior_param}
    , m_effectiveness{effectiveness} {}

double pt__m0(double ft, double kt, double m0) {
  return ft * std::exp(-kt * (m0 - 20));
}

double zmt(double z0t, double kmt, double m0) {
  return z0t + kmt * (m0 - 20);
}

struct PriorCoefficients {
  double m_pt_m0 = 0, m_A = 0, m_alpt = 0, m_zmt = 0;

  static PriorCoefficients T1(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;
    coeff.m_pt_m0 = pt__m0(prior_param.getFt(1), prior_param.getKt(1), m0);
    coeff.m_zmt   = zmt(prior_param.getZ0t(1), prior_param.getKmt(1), m0);
    coeff.m_alpt  = prior_param.getAlphat(1);
    coeff.m_A     = prior_param.getCst(1) * std::pow(coeff.m_zmt, coeff.m_alpt + 1) / coeff.m_alpt;
    return coeff;
  }

  static PriorCoefficients T2(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;
    coeff.m_pt_m0 = pt__m0(prior_param.getFt(2), prior_param.getKt(2), m0);
    coeff.m_zmt   = zmt(prior_param.getZ0t(2), prior_param.getKmt(2), m0);
    coeff.m_alpt  = prior_param.getAlphat(2);
    coeff.m_A     = prior_param.getCst(2) * std::pow(coeff.m_zmt, coeff.m_alpt + 1) / coeff.m_alpt;
    return coeff;
  }

  static PriorCoefficients T3(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;

    double pt_m0_1 = pt__m0(prior_param.getFt(1), prior_param.getKt(1), m0);
    double pt_m0_2 = pt__m0(prior_param.getFt(2), prior_param.getKt(2), m0);

    coeff.m_pt_m0 = 1 - pt_m0_1 - pt_m0_2;
    coeff.m_zmt   = zmt(prior_param.getZ0t(3), prior_param.getKmt(3), m0);
    coeff.m_alpt  = prior_param.getAlphat(3);
    coeff.m_A     = prior_param.getCst(3) * std::pow(coeff.m_zmt, coeff.m_alpt + 1) / coeff.m_alpt;
    return coeff;
  }
};

void NzPrior::computeP_T_z__m0(double m0, const XYDataset::QualifiedName& sed, PhzDataModel::DoubleGrid& grid) {
  const auto&       group_name = m_sedGroupManager.findGroupContaining(sed).first;
  PriorCoefficients coeff;
  if (group_name == "T1") {
    coeff = PriorCoefficients::T1(m0, m_prior_param);
  } else if (group_name == "T2") {
    coeff = PriorCoefficients::T2(m0, m_prior_param);
  } else {
    coeff = PriorCoefficients::T3(m0, m_prior_param);
  }

  // For bright sources, set z > 1 prior to 0, leave the values for z < 1
  if (m0 < 20) {
    for (auto grid_iter = grid.begin(); grid_iter != grid.end(); ++grid_iter) {
      double z = grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
      if (z > 1) {
        *grid_iter = 0;
      }
    }
    return;
  }

  const double pt_m0_div_A = coeff.m_pt_m0 / coeff.m_A;
  const double inv_zmt_pow = std::pow(1. / coeff.m_zmt, coeff.m_alpt);

  for (auto grid_iter = grid.begin(); grid_iter != grid.end(); ++grid_iter) {
    double z = grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
    if (z <= 0.0) {
      z = 1e-4;
    }
    double z_pow = std::pow(z, coeff.m_alpt);
    *grid_iter   = pt_m0_div_A * z_pow * std::exp(-z_pow * inv_zmt_pow);
  }
}

void NzPrior::operator()(PhzDataModel::RegionResults& results) {
  // 1. get the I magnitude
  auto& photometry = results.get<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>();
  auto  flux_ptr   = photometry.get().find(m_i_filter_name.qualifiedName());
  if (flux_ptr == nullptr) {
    throw Elements::Exception() << "NzPrior: Missing filter:" << m_i_filter_name.qualifiedName() << " in the source Photometry";
  }

  auto& posterior_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_LOG_GRID>();

  PhzDataModel::DoubleGrid prior_grid{posterior_grid.getAxesTuple()};
  std::fill(prior_grid.begin(), prior_grid.end(), 1.);

  // Check if the FLAGS are already inserted, if not add it
  if (!results.contains<PhzDataModel::RegionResultType::FLAGS>()) {
    results.set<PhzDataModel::RegionResultType::FLAGS>(std::map<std::string, bool>{});
  }

  if (!flux_ptr->missing_photometry_flag) {
    // flux is in micro Jy the AB mag factor is then 3613E6
    double mag_Iab = -2.5 * std::log10(flux_ptr->flux / 3.631E9);
    auto&  seds    = prior_grid.getAxis<PhzDataModel::ModelParameter::SED>();
    for (auto& sed : seds) {
      auto grid_slice = prior_grid.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);
      computeP_T_z__m0(mag_Iab, sed, grid_slice);
    }

    // Apply the effectiveness to the prior.
    std::transform(prior_grid.begin(), prior_grid.end(), prior_grid.begin(),
                   [this](double v) { return (1 - m_effectiveness) + m_effectiveness * v; });
  } else {
    // The flux needed for computing the N(Z) prior is missing => use a flat prior and flag the source
    results.get<PhzDataModel::RegionResultType::FLAGS>().insert({MISSING_FLUX_FOR_NZ_FLAG, true});
  }

  constexpr double min_value = std::numeric_limits<double>::lowest();
  // Apply the prior to the likelihood
  for (auto l_it = posterior_grid.begin(), p_it = prior_grid.begin(); l_it != posterior_grid.end(); ++l_it, ++p_it) {
    if (*p_it <= 0) {
      *l_it = min_value;
    } else {
      double prior = std::log(*p_it);
      *l_it += prior;
    }
  }

  if (results.get<PhzDataModel::RegionResultType::SAMPLE_SCALE_FACTOR>()) {
    auto& posterior_sampled_grid = results.get<PhzDataModel::RegionResultType::POSTERIOR_SCALING_LOG_GRID>();
    auto  p_it                   = prior_grid.begin();
    for (auto l_s_it = posterior_sampled_grid.begin(); l_s_it != posterior_sampled_grid.end(); ++l_s_it, ++p_it) {
      if (*p_it <= 0) {
        std::fill(l_s_it->begin(), l_s_it->end(), min_value);
      } else {
        double prior = std::log(*p_it);
        for (auto& sample : *l_s_it) {
          sample += prior;
        }
      }
    }
  }
}

}  // namespace PhzNzPrior
}  // namespace Euclid
