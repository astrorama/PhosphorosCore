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
                               const PhzDataModel::PhotometryGrid& phot_grid,
                               std::vector<StaticPriorFunction> static_priors,
                               MarginalizationFunction marginalization_func)
            : m_source_phz_func{std::move(phot_corr_map), phot_grid,
                                std::move(static_priors), marginalization_func} {
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid