/**
 * @file ParallelCatalogHandler.cpp
 * @date February 27, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzUtils/Multithreading.h"

namespace Euclid {
namespace PhzLikelihood {

ParallelCatalogHandler::ParallelCatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                               const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                                               LikelihoodGridFunction likelihood_grid_func,
                                               std::vector<StaticPriorFunction> static_priors,
                                               std::vector<MarginalizationFunction> marginalization_func_list,
                                               bool doNormalizePdf)
        : m_catalog_handler{phot_corr_map, phot_grid_map, std::move(likelihood_grid_func), std::move(static_priors), std::move(marginalization_func_list), doNormalizePdf} { }

ParallelCatalogHandler::~ParallelCatalogHandler() {
  // The multithreaded job is done, so reset the stop threads flag
  PhzUtils::getStopThreadsFlag() = false;
}
}
}
