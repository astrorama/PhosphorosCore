/** 
 * @file ParallelCatalogHandler.cpp
 * @date February 27, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/ParallelCatalogHandler.h"

namespace Euclid {
namespace PhzLikelihood {

ParallelCatalogHandler::ParallelCatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                               const PhzDataModel::PhotometryGrid& phot_grid,
                                               std::vector<StaticPriorFunction> static_priors,
                                               MarginalizationFunction marginalization_func)
        : m_catalog_handler{phot_corr_map, phot_grid, std::move(static_priors), marginalization_func} { }

}
}