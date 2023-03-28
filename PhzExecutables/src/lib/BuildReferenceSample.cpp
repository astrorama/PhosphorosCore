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

#include "PhzExecutables/BuildReferenceSample.h"
#include "ElementsKernel/Logging.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>

#include "PhzConfiguration/BuildReferenceSampleConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/RedshiftFunctorConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzReferenceSample/ReferenceSample.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "PhzModeling/RedshiftFunctor.h"

#include "MathUtils/interpolation/interpolation.h"
#include "XYDataset/CachedProvider.h"

namespace Euclid {
namespace PhzExecutables {

using namespace PhzConfiguration;
using namespace PhzDataModel;
using namespace PhzModeling;
using ReferenceSample::ReferenceSample;

Elements::Logging logger = Elements::Logging::getLogger("BuildReferenceSample");

class DefaultProgressListener {
public:
  void operator()(size_t step, size_t total) {
    int percentage_done = 100 * step / total;
    if (percentage_done > m_last_progress) {
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
      m_last_progress = percentage_done;
    }
  }

private:
  int m_last_progress = -1;
};

/**
 * Look for, and parse the Z-BINS entry on the comment
 * @param comment
 *  The comments stored on the Phosphoros catalog
 * @return
 *  A vector with the knots (bins) of the Z-PDF
 */
static std::vector<double> getPdzBins(std::string comment) {
  boost::replace_all(comment, "\n", "");
  auto z_bins = boost::find_regex(comment, boost::regex{"Z-BINS : \\{[\.0123456789, ]+\\}"});
  if (!z_bins) {
    return {};
  }
  std::string              bins_str{z_bins.begin() + 10, z_bins.end() - 1};
  std::vector<std::string> bins;
  boost::split(bins, bins_str, boost::is_any_of("\t\n ,"));
  std::vector<double> v(bins.size());  	 
  std::transform(bins.begin(), bins.end(), v.begin(), boost::lexical_cast<double, const std::string&>);

  return v;
}

BuildReferenceSample::BuildReferenceSample() : BuildReferenceSample(DefaultProgressListener()) {}

BuildReferenceSample::BuildReferenceSample(ProgressListener progress_listener)
    : m_progress_listener(progress_listener) {}

void BuildReferenceSample::run(Euclid::Configuration::ConfigManager& config_manager) {
  using PhzModeling::NormalizationFunctorFactory;

  auto& igm_function = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();

  auto lum_filter_name = config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
  auto sun_sed_name    = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();

  auto filter_provider  = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
  auto sun_sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
  auto normalizer_functor =
      NormalizationFunctorFactory::GetFunction(filter_provider, lum_filter_name, sun_sed_provider, sun_sed_name);

  XYDataset::CachedProvider reddening_provider{
      config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider()};
  XYDataset::CachedProvider sed_provider{config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider()};

  auto redshiftFunctor = config_manager.getConfiguration<RedshiftFunctorConfig>().getRedshiftFunctor();

  auto ref_sample_config = config_manager.getConfiguration<BuildReferenceSampleConfig>();
  auto ref_sample_path   = ref_sample_config.getReferenceSamplePath();

  logger.info() << "Creating Reference Sample dir";
  if (boost::filesystem::exists(ref_sample_path)){
	  if (ref_sample_config.overwrite()) {
		 std::vector<boost::filesystem::path> paths;
		 for (auto const & entry : boost::filesystem::recursive_directory_iterator(ref_sample_path)) {

			 if (entry.path().extension() == ".npy") {
				 paths.emplace_back(entry);
			 }
		 }
		 logger.info() << "Clearing the Reference Sample dir of "<< paths.size() << " *.npy files";
		 for (auto const & path : paths) {
			 boost::filesystem::remove(path);
		 }

	  } else {
		  throw Elements::Exception() << "The directory already exists: " << ref_sample_path;
	  }
  }
  auto ref_sample = ReferenceSample::create(ref_sample_config.getReferenceSamplePath(), false, ref_sample_config.getMaxSize());

  logger.info() << "Reading the Phosphoros catalog";
  auto phosphoros_readers = ref_sample_config.getPhosphorosCatalogReader();

  logger.info() << "Processing " << phosphoros_readers.size() << " catalogs";

  std::vector<double> pdz_bins;
  size_t              total = 0;
  for (auto& reader : phosphoros_readers) {
    total += reader->rowsLeft();
   logger.info() << "total="<< total;
    auto cat_pdz_bins = getPdzBins(reader->getComment());
   logger.info() << "DZ BIN read";
    if (pdz_bins.empty()) {
      pdz_bins = cat_pdz_bins;
    } else if (pdz_bins != cat_pdz_bins) {
      throw Elements::Exception() << "All catalogs must have the same PDZ bins";
    }
  }
  
  logger.info() << "PDZ BIN read II";

  if (pdz_bins.empty()) {
    logger.warn() << "No PDZ bins found, skipping PDZ processing";
  } else {
    auto log_line = logger.debug() << "PDZ bins: ";
    std::for_each(pdz_bins.begin(), pdz_bins.end(), [&log_line](double bin) {
      log_line << bin << ' ';
    });
  }

  int64_t i = 0;
  for (auto& reader : phosphoros_readers) {
    logger.info() << "Processing input catalog with " << reader->rowsLeft() << " sources";
 
    processCatalog(*reader, ref_sample, igm_function, normalizer_functor, reddening_provider, sed_provider,
                   redshiftFunctor, pdz_bins, total, i);
  }

  logger.info() << "Optimizing the reference sample index";
  ref_sample.optimize();
}

void BuildReferenceSample::processCatalog(Table::TableReader& reader, ReferenceSample& ref_sample,
                                          const PhotometryGridCreator::IgmAbsorptionFunction& igm_function,
                                          const NormalizationFunction&                        normalizer_functor,
                                          XYDataset::XYDatasetProvider&                       reddening_provider,
                                          XYDataset::XYDatasetProvider&                       sed_provider,
                                          const RedshiftFunctor& redshiftFunctor, const std::vector<double>& pdz_bins,
                                          size_t total, int64_t& i) {
  while (reader.hasMoreRows()) {
    auto phosphoros_table = reader.read(10000);
    logger.info() << phosphoros_table.size() << " entries loaded";

    for (auto& object : phosphoros_table) {
      auto                     obj_id = i; // boost::get<int64_t>(object["ID"]);
      auto                     z      = boost::get<double>(object["Z"]);
      auto                     ebv    = boost::get<double>(object["E(B-V)"]);
      auto                     scale  = boost::get<double>(object["Scale"]);
      XYDataset::QualifiedName red_curve_name{boost::get<std::string>(object["ReddeningCurve"])};
      XYDataset::QualifiedName sed_name{boost::get<std::string>(object["SED"])};

      std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> reddening_curve_map;
      std::map<XYDataset::QualifiedName, XYDataset::XYDataset>                 sed_map;

      auto sed = sed_provider.getDataset(sed_name);
      if (!sed) {
        throw Elements::Exception() << "Could not load the SED " << sed_name;
      }
      sed_map.emplace(std::make_pair(sed_name, std::move(*sed)));

      auto red_curve = reddening_provider.getDataset(red_curve_name);
      if (!red_curve) {
        throw Elements::Exception() << "Could not load the reddening curve " << red_curve_name;
      }

      reddening_curve_map.emplace(
          std::make_pair(red_curve_name, interpolatedReddeningCurve(red_curve_name, *red_curve)));

      ModelAxesTuple   grid_axes{createAxesTuple({z}, {ebv}, {red_curve_name}, {sed_name})};
      ModelDatasetGrid grid{grid_axes,           std::move(sed_map), std::move(reddening_curve_map),
                            ExtinctionFunctor{}, redshiftFunctor,    igm_function,
                            normalizer_functor};

      for (auto& cell : grid) {
        std::vector<std::pair<double, double>> scaled_data{};
        for (auto it = cell.begin(); it != cell.end(); ++it) {
          auto scaled_point = *it;
          scaled_point.second *= scale;
          scaled_data.push_back(std::move(scaled_point));
        }
        ref_sample.addSedData(obj_id, XYDataset::XYDataset::factory(std::move(scaled_data)));
      }
      if (!pdz_bins.empty()) {
        auto pdz_vals = boost::get<std::vector<double>>(object["Z-1D-PDF"]);
        ref_sample.addPdzData(obj_id, XYDataset::XYDataset::factory(pdz_bins, pdz_vals));
      }

      ++i;
      m_progress_listener(i, total);
    }
  }
}

std::unique_ptr<MathUtils::Function>
BuildReferenceSample::interpolatedReddeningCurve(const Euclid::XYDataset::QualifiedName& curve_name,
                                                 const Euclid::XYDataset::XYDataset&     curve_data) {
  auto i = m_reddening_cache.find(curve_name);
  if (i == m_reddening_cache.end()) {
    i = m_reddening_cache.emplace(curve_name, std::move(interpolate(curve_data, MathUtils::InterpolationType::LINEAR)))
            .first;
  }
  return i->second->clone();
}

}  // namespace PhzExecutables
}  // namespace Euclid
