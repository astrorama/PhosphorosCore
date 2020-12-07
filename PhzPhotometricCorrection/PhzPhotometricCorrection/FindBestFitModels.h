/**
 * @file PhzPhotometricCorrection/FindBestFitModels.h
 * @date Jan 15, 2015
 * @author Florian Dubath
 */

#ifndef PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H
#define PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H

#include <map>
#include <vector>
#include <memory>
#include "SourceCatalog/Catalog.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/AdjustErrorParamMap.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * @class FindBestFitModels
 *
 * @brief This class is in charge of getting the best fitted models at a given
 * redshift for each sources of a catalog.
 * The redshift to be chosen is provided for each sources by the
 * catalog along with the photometry.
 *
 * @tparam SourceCalculatorFunctor A type of functor encoding the algorithm for
 * finding the best fitted model.
 * See SourcePhzFunctor::SourcePhzFunctor for the signatures.
 */
template<typename SourceCalculatorFunctor>
class FindBestFitModels {
  
public:
  
  using LikelihoodGridFunction = PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction;
  using PriorFunction = PhzLikelihood::SourcePhzFunctor::PriorFunction;
  
  FindBestFitModels(LikelihoodGridFunction likelihood_func,
                    const PhzDataModel::AdjustErrorParamMap& adjust_error_param_map,
                    std::vector<PriorFunction> priors = {},
                    std::vector<PhzLikelihood::SourcePhzFunctor::MarginalizationFunction> marginalization_func_list = {PhzLikelihood::BayesianMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{PhzDataModel::GridType::POSTERIOR}},
                    std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> model_funct_list ={});

  /**
   * @brief Map each input source to the model which is the best match,
   * according their photometries. The results are selected from the models
   * which correspond to the known spectroscopic redshift of the source and
   * the models of other reshdifts are ignored.
   *
   * @param calibration_catalog An object of type SourceCatalog::Catalog,
   * which contains the sources. All the sources are assumed to contain both
   * Photometry and Spec-Z information.
   *
   * @param model_grid_map  An map containing the photometries of the models for
   *    all the parameter space regions
   *
   * @param photometric_correction An object of type
   * PhzDataModel::PhotometricCorrectionMap, containing the photometric corrections
   * for all filters.
   *
   * @return An object of type std::map, with keys of type int64 t,
   * representing the IDs of the sources, and values of type
   * SourceCatalog::Photometry representing the best fitted model for the source.
   *
   * @throws ElementsException
   *    if any of the source is missing the Spec-Z information
   * @throws ElementsException
   *    if any of the source is missing the Photometry information
   */
  template <typename SourceIter>
  std::map<SourceCatalog::Source::id_type, SourceCatalog::Photometry> operator()(
      SourceIter source_begin, SourceIter source_end,
      const std::map<std::string, PhzDataModel::PhotometryGrid>& model_grid_map,
      const PhzDataModel::PhotometricCorrectionMap& photometric_correction) const;
  
private:
  
  const PhzDataModel::AdjustErrorParamMap& m_adjust_error_param_map;
  LikelihoodGridFunction m_likelihood_func;
  std::vector<PriorFunction> m_priors;
  std::vector<PhzLikelihood::SourcePhzFunctor::MarginalizationFunction> m_marginalization_func_list;
  std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> m_model_funct_list;
  
};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#include "PhzPhotometricCorrection/_impl/FindBestFitModels.icpp"

#endif    /* PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H */
