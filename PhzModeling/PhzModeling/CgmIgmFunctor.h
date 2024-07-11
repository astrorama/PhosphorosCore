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
 * @file CgmIgmFunctor.h
 * @date 2024/07/05
 * @author dubathf
 */

#ifndef PHZMODELING_CGMIGMFUNCTOR_H
#define PHZMODELING_CGMIGMFUNCTOR_H

#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
template <typename BaseIgmFunctor>
class CgmIgmFunctor {

public:
  CgmIgmFunctor(double A = 4.92919285, double a = 0.76313514, double c = 17.54936014);

  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed, double z) const;

  static double sigmoid(double z, double A, double a, double c);

  static double sigma_alpha(double nu_rest, double lambda_a, double nu_lyalpha, double C);

  static std::unique_ptr<MathUtils::Function> buildCmgIgmTransmissionFunc(double z, double A, double a, double c);

private:
  BaseIgmFunctor m_BaseIgmFunctor{};
  double         m_A;
  double         m_a;
  double         m_c;
};

}  // end of namespace PhzModeling
}  // end of namespace Euclid

#include "PhzModeling/_impl/CgmIgmFunctor.icpp"

#endif /* PHZMODELING_CGMIGMFUNCTOR_H */
