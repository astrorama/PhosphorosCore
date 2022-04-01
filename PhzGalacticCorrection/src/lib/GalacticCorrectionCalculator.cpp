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

#include "PhzGalacticCorrection/GalacticCorrectionCalculator.h"
#include "MathUtils/interpolation/interpolation.h"
#include <ElementsKernel/Logging.h>
#include <cmath>

using namespace Euclid::MathUtils;

namespace Euclid {
namespace PhzGalacticCorrection {

static auto logger = Elements::Logging::getLogger("GalacticCorrectionCalculator");

static Euclid::XYDataset::XYDataset expDataSet(Euclid::XYDataset::XYDataset const& input, double factor) {
  std::vector<std::pair<double, double>> result(input.size());

  std::transform(input.begin(), input.end(), result.begin(), [factor](const std::pair<double, double>& pair) {
    return std::make_pair(pair.first, std::pow(10., pair.second * factor));
  });

  return Euclid::XYDataset::XYDataset(std::move(result));
}

GalacticCorrectionCalculator::GalacticCorrectionCalculator(const std::vector<PhzDataModel::FilterInfo>& filter_info_vector,
                                                           PhzModeling::ApplyFilterFunctor const&       filter_functor,
                                                           PhzModeling::IntegrateDatasetFunctor const&  integrate_functor,
                                                           XYDataset::XYDataset const&                  red_dataset)
    : m_filter_info(filter_info_vector)
    , m_filter_functor(filter_functor)
    , m_integrate_functor(integrate_functor)
    , m_red_range(red_dataset.front().first, red_dataset.back().first) {

  m_mw_reddening = MathUtils::interpolate(expDataSet(red_dataset, -0.12), MathUtils::InterpolationType::LINEAR);
}

void GalacticCorrectionCalculator::operator()(const XYDataset::XYDataset& model, std::vector<double>& out) const {
  auto filter_iter = m_filter_info.begin();
  for (auto corr_iter = out.begin(); corr_iter != out.end(); ++corr_iter, ++filter_iter) {
    auto   x_filterd  = m_filter_functor(model, filter_iter->getRange(), filter_iter->getFilter());
    double flux_int   = m_integrate_functor(x_filterd, filter_iter->getRange());
    auto   x_reddened = m_filter_functor(x_filterd, m_red_range, *m_mw_reddening);
    double flux_obs   = m_integrate_functor(x_reddened, filter_iter->getRange());

    double a_sed_x = 0.0;

    // if the flux is not null we can compute the correction coef
    if (flux_obs > 0 && flux_int > 0) {
      a_sed_x = -5. * std::log10(flux_obs / flux_int) / 0.6;
    } else {
      logger.debug() << "A correction coefficient where not computed and set to 0. "
                     << "This can be due to SED fully outside of the filter range or may point to negative values in the filter "
                        "transmissions.";
      if (flux_obs < 0 || flux_int < 0) {
        logger.info() << "A computed flux is negative: check your filter transmission for un-physical negatives values! ";
      }
    }
    *corr_iter = a_sed_x;
  }
}

}  // namespace PhzGalacticCorrection
}  // namespace Euclid