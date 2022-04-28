/**
 * @file CatalogHandler.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "PhzLikelihood/CatalogHandler.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

CatalogHandler::CatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                               PhzDataModel::AdjustErrorParamMap adjust_error_param_map,
                               const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                               LikelihoodGridFunction likelihood_grid_func,
                               double sampling_sigma_range,
                               std::vector<PriorFunction> priors,
                               std::vector<MarginalizationFunction> marginalization_func_list,
                               std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> model_funct_list,
                               bool doNormalizePdf)
            : m_source_phz_func{std::move(phot_corr_map),
                                std::move(adjust_error_param_map),
                                phot_grid_map,
                                std::move(likelihood_grid_func),
                                sampling_sigma_range,
                                std::move(priors),
                                std::move(marginalization_func_list),
                                std::move(model_funct_list),
                                doNormalizePdf} {
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
