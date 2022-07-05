/*
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

/**
 * @file PhzExecutables/ComputeReferenceSample.h
 * @date 08/13/18
 * @author aalvarez
 */

#ifndef _PHZEXECUTABLES_COMPUTEREFERENCESAMPLE_H
#define _PHZEXECUTABLES_COMPUTEREFERENCESAMPLE_H

#include "Configuration/ConfigManager.h"
#include <functional>

#include "MathUtils/function/Function.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzReferenceSample/ReferenceSample.h"
#include "Table/TableReader.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzExecutables {

/**
 * @class ComputeReferenceSample
 * @brief
 *
 */
class BuildReferenceSample {

public:
  typedef std::function<void(size_t step, size_t total)> ProgressListener;

  /**
   * @brief Destructor
   */
  virtual ~BuildReferenceSample() = default;

  BuildReferenceSample();
  BuildReferenceSample(ProgressListener progress_listener);

  void run(Configuration::ConfigManager& config_manager);

private:
  ProgressListener                                                         m_progress_listener;
  std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> m_reddening_cache;

  std::unique_ptr<MathUtils::Function> interpolatedReddeningCurve(const XYDataset::QualifiedName& curve_name,
                                                                  const XYDataset::XYDataset&     curve_data);

  void processCatalog(Euclid::Table::TableReader& reader, ReferenceSample::ReferenceSample& ref_sample,
                      const PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction&        igm_function,
                      const std::function<XYDataset::XYDataset(const XYDataset::XYDataset&)>& normalizer_functor,
                      XYDataset::XYDatasetProvider& reddening_provider, XYDataset::XYDatasetProvider& sed_provider,
                      const PhzModeling::RedshiftFunctor& redshiftFunctor, const std::vector<double>& pdz_bins,
                      size_t total, int64_t& i);
};  // End of ComputeReferenceSample class

}  // namespace PhzExecutables
}  // namespace Euclid

#endif
