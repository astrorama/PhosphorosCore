/**
 * @file src/lib/NzPrior.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */
 
 // From: Benitez https://arxiv.org/abs/astro-ph/9811189v1

#include "PhzNzPrior/NzPrior.h"

#include <cmath>
#include <iterator>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzNzPrior {

static Elements::Logging logger = Elements::Logging::getLogger("NzPrior");

static const std::string MISSING_FLUX_FOR_NZ_FLAG{"MISSING_FLUX_FOR_NZ_FLAG"};

NzPrior::NzPrior(const PhzDataModel::QualifiedNameGroupManager& sedGroupManager,
                 const XYDataset::QualifiedName& i_filter_name, const NzPriorParam& prior_param, double effectiveness)
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
  double m_pt_m0 = 0, m_A = 0, m_alpt = 0, m_inv_zmt_pow = 0;

  static PriorCoefficients T1(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;
    double            zmtv = zmt(prior_param.getZ0t(1), prior_param.getKmt(1), m0);
    coeff.m_pt_m0          = pt__m0(prior_param.getFt(1), prior_param.getKt(1), m0);
    coeff.m_alpt           = prior_param.getAlphat(1);
    coeff.m_A              = prior_param.getCst(1) * std::pow(zmtv, coeff.m_alpt + 1) / coeff.m_alpt;
    coeff.m_inv_zmt_pow    = std::pow(1. / zmtv, coeff.m_alpt);
    return coeff;
  }

  static PriorCoefficients T2(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;
    double            zmtv = zmt(prior_param.getZ0t(2), prior_param.getKmt(2), m0);

    coeff.m_pt_m0       = pt__m0(prior_param.getFt(2), prior_param.getKt(2), m0);
    coeff.m_alpt        = prior_param.getAlphat(2);
    coeff.m_A           = prior_param.getCst(2) * std::pow(zmtv, coeff.m_alpt + 1) / coeff.m_alpt;
    coeff.m_inv_zmt_pow = std::pow(1. / zmtv, coeff.m_alpt);
    return coeff;
  }

  static PriorCoefficients T3(double m0, const NzPriorParam& prior_param) {
    PriorCoefficients coeff;

    double pt_m0_1 = pt__m0(prior_param.getFt(1), prior_param.getKt(1), m0);
    double pt_m0_2 = pt__m0(prior_param.getFt(2), prior_param.getKt(2), m0);
    double zmtv    = zmt(prior_param.getZ0t(3), prior_param.getKmt(3), m0);

    coeff.m_pt_m0       = 1 - pt_m0_1 - pt_m0_2;
    coeff.m_alpt        = prior_param.getAlphat(3);
    coeff.m_A           = prior_param.getCst(3) * std::pow(zmtv, coeff.m_alpt + 1) / coeff.m_alpt;
    coeff.m_inv_zmt_pow = std::pow(1. / zmtv, coeff.m_alpt);
    return coeff;
  }
};

static void computeP_T_z__m0(std::array<PriorCoefficients, 3>& coeffs, std::vector<int> coeff_index,
                             PhzDataModel::DoubleGrid& grid) {
  for (auto grid_iter = grid.begin(); grid_iter != grid.end(); ++grid_iter) {
    auto z       = grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
    auto sed_idx = grid_iter.axisIndex<PhzDataModel::ModelParameter::SED>();

    if (z <= 0.0) {
      z = 1e-4;
    }

    const auto& coeff = coeffs[coeff_index[sed_idx]];
    double      z_pow = std::pow(z, coeff.m_alpt);
    *grid_iter        = (coeff.m_pt_m0 / coeff.m_A) * z_pow * std::exp(-z_pow * coeff.m_inv_zmt_pow);
  }
}

void NzPrior::operator()(PhzDataModel::RegionResults& results) {
  // 1. get the I magnitude
  auto& photometry = results.get<PhzDataModel::RegionResultType::SOURCE_PHOTOMETRY_REFERENCE>();
  auto  flux_ptr   = photometry.get().find(m_i_filter_name.qualifiedName());
  if (flux_ptr == nullptr) {
    throw Elements::Exception() << "NzPrior: Missing filter:" << m_i_filter_name.qualifiedName()
                                << " in the source Photometry";
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

    // Precompute the coefficients for the three group of seds
    std::array<PriorCoefficients, 3> coeffs{PriorCoefficients::T1(mag_Iab, m_prior_param),
                                            PriorCoefficients::T2(mag_Iab, m_prior_param),
                                            PriorCoefficients::T3(mag_Iab, m_prior_param)};

    // The Z axis changes more slowly than the SED axis, so it is interesting to have the inner loop
    // iterate over the SEDs for memory locality, but we need to know to which group the SED correspond
    // to know which set of coefficients to use. To avoid a map lookup, we populate a vector with the coefficient
    // index corresponding to each SED.
    auto&            seds = prior_grid.getAxis<PhzDataModel::ModelParameter::SED>();
    std::vector<int> coeff_index(seds.size());
    for (size_t i = 0; i < seds.size(); ++i) {
      const auto& group_name = m_sedGroupManager.findGroupContaining(seds[i]).first;
      if (group_name == "T1") {
        coeff_index[i] = 0;
      } else if (group_name == "T2") {
        coeff_index[i] = 1;
      } else {
        coeff_index[i] = 2;
      }
    }

    // For bright sources, set z > 1 prior to 0, leave the values for z < 1
    if (mag_Iab < 20) {
      for (auto grid_iter = prior_grid.begin(); grid_iter != prior_grid.end(); ++grid_iter) {
        double z = grid_iter.axisValue<PhzDataModel::ModelParameter::Z>();
        if (z > 1) {
          *grid_iter = 0;
        }
      }
    } else {
      computeP_T_z__m0(coeffs, coeff_index, prior_grid);
    }

    // Apply the effectiveness to the prior.
    std::transform(prior_grid.begin(), prior_grid.end(), prior_grid.begin(), [this](double v) {
      return (1 - m_effectiveness) + m_effectiveness * v;
    });
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
