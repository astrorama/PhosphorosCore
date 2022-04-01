/**
 * @copyright (C) 2022 Euclid Science Ground Segment
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

#ifndef _PHZGALACTICCORRECTION_GALACTICCORRECTIONCALCULATOR_H
#define _PHZGALACTICCORRECTION_GALACTICCORRECTIONCALCULATOR_H

#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/XYDataset.h"
#include <memory>

namespace Euclid {
namespace PhzGalacticCorrection {

class GalacticCorrectionCalculator {
public:
  GalacticCorrectionCalculator(std::vector<PhzDataModel::FilterInfo> const& filter_info_vector,
                               PhzModeling::ApplyFilterFunctor const&       filter_functor,
                               PhzModeling::IntegrateDatasetFunctor const&  integrate_functor,
                               XYDataset::XYDataset const&                  red_dataset);

  void operator()(XYDataset::XYDataset const& model, std::vector<double>& out) const;

private:
  std::unique_ptr<MathUtils::Function>         m_mw_reddening;
  std::vector<PhzDataModel::FilterInfo> const& m_filter_info;
  PhzModeling::ApplyFilterFunctor const&       m_filter_functor;
  PhzModeling::IntegrateDatasetFunctor const&  m_integrate_functor;
  std::pair<double, double>                    m_red_range;
};

}  // namespace PhzGalacticCorrection
}  // namespace Euclid

#endif  // _PHZGALACTICCORRECTION_GALACTICCORRECTIONCALCULATOR_H
