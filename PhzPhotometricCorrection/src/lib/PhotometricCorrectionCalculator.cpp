/** 
 * @file PhotometricCorrectionCalculator.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Exception.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzUtils/Multithreading.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzPhotometricCorrection {


static Elements::Logging logger = Elements::Logging::getLogger("PhotometricCorrectionCalculator");

PhotometricCorrectionCalculator::PhotometricCorrectionCalculator(
                    FindBestFitModelsFunction find_best_fit_models,
                    CalculateScaleFactorsMapFunction calculate_scale_factors_map,
                    CalculatePhotometricCorrectionFunction calculate_photometric_correction)
        : m_find_best_fit_models{std::move(find_best_fit_models)},
          m_calculate_scale_factors_map{std::move(calculate_scale_factors_map)},
          m_calculate_photometric_correction{std::move(calculate_photometric_correction)} {
}

PhotometricCorrectionCalculator::~PhotometricCorrectionCalculator() {
  PhzUtils::getStopThreadsFlag() = false;
}

PhzDataModel::PhotometricCorrectionMap createInitialPhotCorr(const SourceCatalog::Catalog& catalog) {
  PhzDataModel::PhotometricCorrectionMap phot_corr {};
  auto source_phot = catalog.begin()->getAttribute<SourceCatalog::Photometry>();
  for (auto iter=source_phot->begin(); iter!=source_phot->end(); ++iter) {
    phot_corr.emplace(iter.filterName(), 1.);
  }
  return phot_corr;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionCalculator::operator ()(
                              const SourceCatalog::Catalog& catalog,
                              const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
                              StopCriteriaFunction stop_criteria_func,
                              SelectorFunction selector,
                              ProgressListener progress_listener) {
  PhzDataModel::PhotometricCorrectionMap phot_corr = createInitialPhotCorr(catalog);
  size_t counter {0};
  while (!stop_criteria_func(phot_corr)) {
    if (PhzUtils::getStopThreadsFlag()) {
      throw Elements::Exception() << "Stopped by the user";
    }
    auto best_fit_model_map = m_find_best_fit_models(
                                            catalog, model_grid_map, phot_corr);
    auto scale_factor_map = m_calculate_scale_factors_map(
                                            catalog.begin(), catalog.end(),
                                            best_fit_model_map);
    phot_corr = m_calculate_photometric_correction(
                                            catalog.begin(), catalog.end(),
                                            scale_factor_map, best_fit_model_map,
                                            selector);

    if (progress_listener) {
      progress_listener(++counter, phot_corr);
      logger.info() << "Iteration :" << counter;
    }

    // output correction for debug
    logger.debug() << "Corrections:";
    for (auto& item : phot_corr) {
        logger.info() << item.first << " : " << item.second;
    }
  }
  return phot_corr;
}

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid
