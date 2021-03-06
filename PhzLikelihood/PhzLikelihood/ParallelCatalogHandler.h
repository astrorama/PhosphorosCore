/**
 * @file ParallelCatalogHandler.h
 * @date February 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H
#define PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H

#include "PhzLikelihood/CatalogHandler.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class ParallelCatalogHandler
 *
 * @brief Wrapper class around the CatalogHandler class, which enables parallel
 * processing of the sources.
 */
class ParallelCatalogHandler {

public:
  typedef CatalogHandler::MarginalizationFunction MarginalizationFunction;

  typedef CatalogHandler::PriorFunction StaticPriorFunction;

  typedef CatalogHandler::LikelihoodGridFunction LikelihoodGridFunction;

  typedef CatalogHandler::ProgressListener ProgressListener;

  /**
   * Constructs a new ParallelCatalogHandler instance. If the given photometric
   * correction map does not contain corrections for all the filters of the
   * given model photometries an exception is thrown.
   *
   * @param phot_corr_map
   *    A map with the photometric corrections to be applied for each filter
   * @param adjust_error_param_map
   *    The map with the parameter for recomputing the errors
   * @param phot_grid_map
   *    The const reference to the map containing the grids with the model
   *    photometries for all the parameter space regions
   * @param likelihood_grid_func
   *    The function to use for computing the likelihood grid for a single source
   * @param static_priors
   *    The static priors to apply on the likelihood grid
   * @param marginalization_func_list
   *    The functions to use for marginalizing the multi-dimensional likelihood
   *    grid to a 1D PDFs
   * @throws ElementsException
   *    If the phot_corr_map does not contain photometric corrections for all
   *    the filters of the model photometries
   */
  ParallelCatalogHandler(PhzDataModel::PhotometricCorrectionMap                     phot_corr_map,
                         PhzDataModel::AdjustErrorParamMap                          adjust_error_param_map,
                         const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                         LikelihoodGridFunction likelihood_grid_func, double sampling_sigma_range,
                         std::vector<StaticPriorFunction>                                     static_priors,
                         std::vector<MarginalizationFunction>                                 marginalization_func_list,
                         std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> model_funct_list,
                         bool                                                                 doNormalizePdf = true);

  virtual ~ParallelCatalogHandler();

  /**
   * Iterates through a set of sources and calculates the PHZ parameters for
   * each of them, using all the available threads in parallel. The assumption
   * that all the sources contain the Photometry attribute is made. The progress
   * of the catalog handling can be observed by using a ProgressListener, which
   * will be notified every 0.1 sec. The output is provided by calls to the
   * given OutputHandler. There is one call performed for each source and the
   * order of the calls is the same as the order of the sources in the input
   * catalog. For this reason, these calls are executed after all threads have
   * finished their jobs.
   *
   * @tparam SourceIter
   *    The type of iterator over the sources. It must be an iterator over
   *    objects of type SourceCatalog::Source
   * @param source_begin
   *    An iterator pointing to the first source
   * @param source_end
   *    An iterator pointing to one after the last source
   * @param out_handler
   *    A reference to the PhzOutput::OutputHandler object, which will be used
   *    for returning the output.
   * @param progress_listener
   *    A function of type ProgressListener which is notified with the progress
   *    of the catalog handling (defaults to no action)
   */
  template <typename SourceIter>
  void handleSources(SourceIter source_begin, SourceIter source_end, PhzOutput::OutputHandler& out_handler,
                     ProgressListener progress_listener = ProgressListener{}) const;

private:
  CatalogHandler m_catalog_handler;
};

}  // namespace PhzLikelihood
}  // namespace Euclid

#include "PhzLikelihood/_impl/ParallelCatalogHandler.icpp"

#endif /* PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H */
