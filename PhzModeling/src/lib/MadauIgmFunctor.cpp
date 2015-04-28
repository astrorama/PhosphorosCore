/** 
 * @file IgmAbsorptionFunctor.cpp
 * @date March 16, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include "PhzModeling/MadauIgmFunctor.h"

namespace Euclid {
namespace PhzModeling {

static const std::map<int, double> LymanA {
  {2, 3.6E-3},
  {3, 1.7E-3},
  {4, 1.2E-3},
  {5, 9.3E-4}
};

static const std::map<int, double> LymanLambda {
  {2, 1216},
  {3, 1026},
  {4, 973},
  {5, 950}
};

double d_a(double z) {
  double low = 1050. * (1+z);
  double high = 1170. * (1+z);
  double step = (high-low)/100.;
  std::vector<double> lambda;
  std::vector<double> exp_ta;
  for (double x=low; x<=high; x+=step) {
    lambda.push_back(x);
    double ta = LymanA.at(2) * std::pow(x/LymanLambda.at(2), 3.46);
    exp_ta.push_back(std::exp(-ta));
  }
  double integral = 0;
  for (size_t i=0; i<lambda.size()-1; ++i) {
    integral += (lambda[i+1]-lambda[i]) * (exp_ta[i]+exp_ta[i+1]) / 2;
  }
  return 1 - integral / (120*(1+z));
}

double d_b(double z) {
  double low = 920. * (1+z);
  double high = 1015. * (1+z);
  double step = (high-low)/100.;
  std::vector<double> lambda;
  std::vector<double> exp_tb;
  for (double x=low; x<=high; x+=step) {
    lambda.push_back(x);
    double tb = 0;
    for (int i=2; i<=5; ++i) {
      tb += LymanA.at(i) * std::pow(x/LymanLambda.at(i), 3.46);
    }
    exp_tb.push_back(std::exp(-tb));
  }
  double integral = 0;
  for (size_t i=0; i<lambda.size()-1; ++i) {
    integral += (lambda[i+1]-lambda[i]) * (exp_tb[i]+exp_tb[i+1]) / 2;
  }
  return 1 - integral / (95*(1+z));
}

XYDataset::XYDataset MadauIgmFunctor::operator()(const XYDataset::XYDataset& sed,
                                                      double z) const {
  auto da_iter = m_da_cache.find(z);
  if (da_iter == m_da_cache.end()) {
    m_da_cache[z] = d_a(z);
    da_iter = m_da_cache.find(z);
  }
  auto db_iter = m_db_cache.find(z);
  if (db_iter == m_db_cache.end()) {
    m_db_cache[z] = d_b(z);
    db_iter = m_db_cache.find(z);
  }
  double zero_step = 912. * (1+z);
  double beta_step = 1026. * (1+z);
  double alpha_step = 1216. * (1+z);
  std::vector<std::pair<double, double>> absorbed_values {};
  for (auto& sed_pair : sed) {
    double new_value = sed_pair.second;
    if (sed_pair.first < zero_step) {
      new_value = 0;
    } else if (sed_pair.first < beta_step) {
      new_value *= 1 - da_iter->second;
    } else if (sed_pair.first < alpha_step) {
      new_value *= 1 - db_iter->second;
    }
    absorbed_values.emplace_back(sed_pair.first, new_value);
  }
  return XYDataset::XYDataset {std::move(absorbed_values)};
}


} // end of namespace PhzModeling
} // end of namespace Euclid