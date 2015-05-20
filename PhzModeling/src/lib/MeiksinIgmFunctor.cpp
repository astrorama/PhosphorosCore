/** 
 * @file MeiksinIgmFunctor.cpp
 * @date April 28, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include <vector>
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/MeiksinIgmFunctor.h"

namespace Euclid {
namespace PhzModeling {

static const std::vector<double> lyman_lambda {
  1215.43603768, 
  1025.52415679, 
  972.348830143, 
  949.559404436, 
  937.622086209, 
  930.568216348, 
  926.046504898, 
  922.971741112, 
  920.784877029, 
  919.173503494, 
  917.951692792, 
  917.003081999, 
  916.25178225, 
  915.646568564, 
  915.151840134, 
  914.742226274, 
  914.399248161, 
  914.109186671, 
  913.861682465, 
  913.648794232, 
  913.464351299, 
  913.303499903, 
  913.162379612, 
  913.037888881, 
  912.927512745, 
  912.829194506, 
  912.741239023, 
  912.662239007, 
  912.591018279, 
  912.526587663
};

static const std::vector<double> lyman_coef_3_5 {
  0.348, 0.179, 0.109
};

static const std::vector<double> lyman_coef_6_9 {
  0.0722, 0.0508, 0.0373, 0.0283
};

static const std::vector<int> n_factorial {
  1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800
};

std::unique_ptr<MathUtils::Function> blue_values = MathUtils::interpolate(
    {0.0, 0.315789473684, 0.631578947368, 0.947368421053, 1.26315789474, 1.57894736842,
        1.89473684211, 2.21052631579, 2.52631578947, 2.84210526316, 3.15789473684,
        3.47368421053, 3.78947368421, 4.10526315789, 4.42105263158, 4.73684210526,
        5.05263157895, 5.36842105263, 5.68421052632, 6.0},
    {0.843224330232, 0.734240597116, 0.610296789367, 0.482865694713, 0.362704079899,
        0.258009634098, 0.173394571004, 0.109834986133, 0.0654280060046, 0.0365706756963,
        0.0191376528015, 0.00935574769107, 0.00426338729084, 0.00180705684965,
        0.000710859380918, 0.000258965877602, 8.71759912257e-05, 2.70576696332e-05,
        7.72610476565e-06, 2.02621914478e-06},
    MathUtils::InterpolationType::LINEAR
);

double ta(double zn) {
  return (zn <= 4) ? (0.00211 * pow(1.+zn, 3.7)) : (0.00058 * pow(1.+zn, 4.5));
}

double t_lyman_series(double z, double l) {
  double t = 0.;
  if (l > lyman_lambda[0]*(1.+z)) {
      return t;
  }
  // n=2
  double zn = (l / lyman_lambda[0]) - 1.;
  t += ta(zn);
  // n=3-5
  for (auto l_iter=lyman_lambda.begin()+1, c_iter=lyman_coef_3_5.begin();
                      c_iter!=lyman_coef_3_5.end(); ++l_iter, ++ c_iter) {
    if (l > *l_iter*(1.+z)) {
      return t;
    }
    zn = (l / *l_iter) - 1.;
    t += ta(zn) * *c_iter * std::pow(0.25*(1+zn), (zn<=3 ? 1./3. : 1./6.));
  }
  // n=6-9
  for (auto l_iter=lyman_lambda.begin()+4, c_iter=lyman_coef_6_9.begin();
                      c_iter!=lyman_coef_6_9.end(); ++l_iter, ++ c_iter) {
    if (l > *l_iter*(1.+z)) {
      return t;
    }
    zn = (l / *l_iter) - 1.;
    t += ta(zn) * *c_iter * std::pow(0.25*(1+zn), 1./3.);
  }
  // n=10-31
  int n = 10;
  for (auto l_iter=lyman_lambda.begin()+8; l_iter!=lyman_lambda.end(); ++l_iter, ++n) {
    if (l > *l_iter*(1.+z)) {
      return t;
    }
    zn = (l / *l_iter) - 1.;
    double ttheta = ta(zn) * lyman_coef_6_9.back() * std::pow(0.25*(1+zn), 1./3.);
    t += ttheta * 720. / (n * (n*n - 1.));
  }
  return t;
}

double t_ots(double z, double l) {
  if (l > 912.*(1.+z)) {
      return 0.;
  }
  double zl = (l / 912.) - 1.;
  return 0.805 * (1+zl)*(1+zl)*(1+zl) * (1/(1+zl) - 1/(1+z));
}

double t_lls(double z, double l) {
  double l_div = l / 912.;
  if (l_div > (1.+z)) {
      return 0.;
  }
  double sum = 0.;
  for (int n=1; n<=11; ++n) {
    sum += 0.5 * std::pow(-1, n) / (3*n-2.5) / (n-0.5) / n_factorial[n] *
           (std::pow(1+z, 2.5-3*n) * std::pow(l_div, 3*n) - std::pow(l_div, 2.5));
  }
  return 0.443113462449 * ((1+z) * std::pow(l_div, 1.5) - std::pow(l_div, 2.5)) - 0.25 * sum;
}

XYDataset::XYDataset MeiksinIgmFunctor::operator()(const XYDataset::XYDataset& sed,
                                                      double z) const {
  double blue_fix_step = 567. * (1+z);
  double blue_fix_value = (*blue_values)(z);
  double one_step = lyman_lambda[0] * (1+z);
  std::vector<std::pair<double, double>> absorbed_values {};
  for (auto& sed_pair : sed) {
    double new_value = sed_pair.second;
    if (sed_pair.first <= blue_fix_step) {
      new_value *= blue_fix_value;
    } else if (sed_pair.first <= one_step) {
      double t = t_lyman_series(z, sed_pair.first);
      t += t_ots(z, sed_pair.first);
      t += t_lls(z, sed_pair.first);
      new_value *= std::exp(-t);
    }
    absorbed_values.emplace_back(sed_pair.first, new_value);
  }
  return XYDataset::XYDataset {std::move(absorbed_values)};
}


} // end of namespace PhzModeling
} // end of namespace Euclid