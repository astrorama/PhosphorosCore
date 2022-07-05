/**
 * @file PhotometricCorrectionCalculator.h
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H
#define PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H

#include "PhotometricCorrectionAlgorithm.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"
#include "SourceCatalog/Catalog.h"
#include <functional>
#include <map>
#include <string>

namespace Euclid {
namespace PhzPhotometricCorrection {

class PhotometricCorrectionCalculator {

public:
  typedef std::function<void(size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& phot_corr)> ProgressListener;
  typedef SourceCatalog::Source::id_type                                                               source_id_type;

  typedef std::function<std::map<source_id_type, SourceCatalog::Photometry>(
      SourceCatalog::Catalog::const_iterator source_begin, SourceCatalog::Catalog::const_iterator source_end,
      const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
      const PhzDataModel::PhotometricCorrectionMap&              photometric_correction)>
      FindBestFitModelsFunction;

  typedef std::function<std::map<source_id_type, double>(
      SourceCatalog::Catalog::const_iterator source_begin, SourceCatalog::Catalog::const_iterator source_end,
      const std::map<source_id_type, SourceCatalog::Photometry>& model_phot_map)>
      CalculateScaleFactorsMapFunction;

  typedef PhzPhotometricCorrection::PhotometricCorrectionAlgorithm::PhotometricCorrectionSelector<
      SourceCatalog::Catalog::const_iterator>
      SelectorFunction;

  typedef std::function<PhzDataModel::PhotometricCorrectionMap(
      SourceCatalog::Catalog::const_iterator source_begin, SourceCatalog::Catalog::const_iterator source_end,
      const std::map<source_id_type, double>&                    scale_factor_map,
      const std::map<source_id_type, SourceCatalog::Photometry>& model_phot_map, SelectorFunction selector)>
      CalculatePhotometricCorrectionFunction;

  typedef std::function<bool(const PhzDataModel::PhotometricCorrectionMap& phot_corr)> StopCriteriaFunction;

  PhotometricCorrectionCalculator(FindBestFitModelsFunction              find_best_fit_models,
                                  CalculateScaleFactorsMapFunction       calculate_scale_factors_map,
                                  CalculatePhotometricCorrectionFunction calculate_photometric_correction);

  virtual ~PhotometricCorrectionCalculator();

  PhzDataModel::PhotometricCorrectionMap
  operator()(const SourceCatalog::Catalog&                              catalog,
             const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
             StopCriteriaFunction stop_criteria_func, SelectorFunction selector,
             ProgressListener progress_listener = ProgressListener{});

private:
  FindBestFitModelsFunction              m_find_best_fit_models;
  CalculateScaleFactorsMapFunction       m_calculate_scale_factors_map;
  CalculatePhotometricCorrectionFunction m_calculate_photometric_correction;
};

}  // end of namespace PhzPhotometricCorrection
}  // end of namespace Euclid

#endif /* PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H */
