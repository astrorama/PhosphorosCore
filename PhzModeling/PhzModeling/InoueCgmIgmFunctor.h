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
 * @file InoueCgmIgmFunctor.h
 * @date 2024/07/05
 * @author dubathf
 */

#ifndef PHZMODELING_INOUECGMIGMFUNCTOR_H
#define PHZMODELING_INOUECGMIGMFUNCTOR_H

#include "MathUtils/interpolation/interpolation.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/// Applies the IGM absorption to an already redshifted SED template.
class InoueCgmIgmFunctor {

public:
  XYDataset::XYDataset operator()(const XYDataset::XYDataset& sed, double z) const;

  static double sigmoid(double z, double A, double a, double c);

  static double sigma_alpha(double nu_rest, double lambda_a, double nu_lyalpha, double C);

  static std::unique_ptr<MathUtils::Function> buildCmgIgmTransmissionFunc(double z);

private:
  InoueIgmFunctor m_InoueIgmFunctor{};
};

}  // end of namespace PhzModeling
}  // end of namespace Euclid

#endif /* PHZMODELING_INOUECGMIGMFUNCTOR_H */
