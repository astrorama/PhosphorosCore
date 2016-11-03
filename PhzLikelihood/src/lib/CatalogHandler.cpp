/** 
 * @file CatalogHandler.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Logging.h"
#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzLikelihood {

CatalogHandler::CatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                               const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                               LikelihoodGridFunction likelihood_grid_func,
                               std::vector<PriorFunction> priors,
                               std::vector<MarginalizationFunction> marginalization_func_list)
            : m_source_phz_func{std::move(phot_corr_map), phot_grid_map, std::move(likelihood_grid_func),
                                std::move(priors), std::move(marginalization_func_list)} {
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid