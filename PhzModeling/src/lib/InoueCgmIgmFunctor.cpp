/**
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file InoueCgmIgmFunctor.cpp
 * @date May 6, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/InoueCgmIgmFunctor.h"
#include "ElementsKernel/Logging.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include <cmath>
#include <limits>
#include <map>
#include <mutex>
#include <vector>

#include <iostream>

namespace Euclid {
namespace PhzModeling {

static Elements::Logging logger = Elements::Logging::getLogger("InoueCgmIgmFunctor");

constexpr double pi = 3.14159265358979323846;

double InoueCgmIgmFunctor::sigmoid(double z, double A, double a, double c) {
  return c + A / (1.0 + std::exp(-a * (z - 6.0)));
}

double InoueCgmIgmFunctor::sigma_alpha(double nu_rest, double lambda_a, double nu_lyalpha, double C) {
  return 1e-16 * C * std::pow(nu_rest / nu_lyalpha, 4) /
         (4 * std::pow(pi * (nu_rest - nu_lyalpha), 2) +
          0.25 * lambda_a * lambda_a * std::pow(nu_rest / nu_lyalpha, 6));
}

std::unique_ptr<MathUtils::Function> InoueCgmIgmFunctor::buildCmgIgmTransmissionFunc(double z) {
  double N_HI = std::pow(10, sigmoid(z, 4.92919285, 0.76313514, 17.54936014));

  std::vector<double> x_list{};
  std::vector<double> y_list{};

  double lambda_min   = 1000;
  double lambda_max   = 15000;
  double delta_lambda = 3;

  for (double lambda = lambda_min; lambda <= lambda_max; lambda += delta_lambda) {
    x_list.push_back(lambda);
    double tau_i = N_HI * sigma_alpha(3e18 * (1 + z) / lambda, 6.255486e8, 2.46607e15, 6.9029528e22);
    y_list.push_back(std::exp(-tau_i));
  }

  return MathUtils::interpolate(x_list, y_list, MathUtils::InterpolationType::LINEAR);
}

static std::map<double, std::unique_ptr<MathUtils::Function>> igm_cmg_transmission_map{};
static std::mutex                                             igm_cmg_func_mutex{};

static MathUtils::Function& getCgmIgmTransmissionFunc(double z) {
  std::lock_guard<std::mutex> guard{igm_cmg_func_mutex};
  auto&                       func_ptr = igm_cmg_transmission_map[z];
  if (func_ptr == nullptr) {
    func_ptr = InoueCgmIgmFunctor::buildCmgIgmTransmissionFunc(z);
  }
  return *func_ptr;
}

XYDataset::XYDataset InoueCgmIgmFunctor::operator()(const XYDataset::XYDataset& sed, double z) const {

  auto sed_inoue = m_InoueIgmFunctor(sed, z);

  if (z < 6) {
    return sed_inoue;
  } else {
    std::vector<std::pair<double, double>> absorbed_values{};
    MathUtils::Function&                   igm_func = getCgmIgmTransmissionFunc(z);

    for (auto& sed_pair : sed_inoue) {
      absorbed_values.emplace_back(sed_pair.first,
                                   sed_pair.second * igm_func(0.1 * sed_pair.first));  // Computation is in Angstrom
                                                                                       // while SED are in nm
    }

    return XYDataset::XYDataset{std::move(absorbed_values)};
  }
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
