/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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


/*
 * @file Pdf1DTraits.h
 * @author Nikolaos Apostolakos
 */

#ifndef LIKELIHOODPDF1DTRAITS_H
#define LIKELIHOODPDF1DTRAITS_H

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

template <int FixedAxis>
struct LikelihoodPdf1DTraits {
};

template <>
struct LikelihoodPdf1DTraits<PhzDataModel::ModelParameter::SED> {
  static constexpr auto PdfRes = PhzDataModel::RegionResultType::LIKELIHOOD_SED_1D_PDF;
};

template <>
struct LikelihoodPdf1DTraits<PhzDataModel::ModelParameter::REDDENING_CURVE> {
  static constexpr auto PdfRes = PhzDataModel::RegionResultType::LIKELIHOOD_RED_CURVE_1D_PDF;
};

template <>
struct LikelihoodPdf1DTraits<PhzDataModel::ModelParameter::EBV> {
  static constexpr auto PdfRes = PhzDataModel::RegionResultType::LIKELIHOOD_EBV_1D_PDF;
};

template <>
struct LikelihoodPdf1DTraits<PhzDataModel::ModelParameter::Z> {
  static constexpr auto PdfRes = PhzDataModel::RegionResultType::LIKELIHOOD_Z_1D_PDF;
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif /* PDF1DTRAITS_H */

