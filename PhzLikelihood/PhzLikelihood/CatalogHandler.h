/**
 * @file CatalogHandler.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_CATALOGHANDLER_H
#define PHZLIKELIHOOD_CATALOGHANDLER_H

#include "PhzDataModel/AdjustErrorParamMap.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class CatalogHandler
 *
 * @brief Class responsible for organizing the PHZ calculation for a set of
 * sources
 *
 * @details
 * This class is using for this a fitting algorithm between the source photometries
 * and a set of model photometries. It also applies photometric correction for
 * the filter zeroset to the photometries of the sources.
 */
class CatalogHandler {

public:
  typedef SourcePhzFunctor::MarginalizationFunction MarginalizationFunction;

  typedef SourcePhzFunctor::PriorFunction PriorFunction;

  typedef SourcePhzFunctor::LikelihoodGridFunction LikelihoodGridFunction;

  /**
   * Defines the signature of the functions which can be used as listeners for
   * the progress of the catalog handling. The first parameter is the
   * number of the current step and the second is the total number of steps.
   */
  typedef std::function<void(size_t step, size_t total)> ProgressListener;

  /**
   * Constructs a new CatalogHandler instance. If the given photometric
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
   * @param priors
   *    The priors to apply on the likelihood grid
   * @param marginalization_func_list
   *    The functions to use for marginalizing the multi-dimensional likelihood
   *    grid to a 1D PDFs
   * @throws ElementsException
   *    If the phot_corr_map does not contain photometric corrections for all
   *    the filters of the model photometries
   */
  CatalogHandler(PhzDataModel::PhotometricCorrectionMap                     phot_corr_map,
                 PhzDataModel::AdjustErrorParamMap                          adjust_error_param_map,
                 const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                 LikelihoodGridFunction likelihood_grid_func, double sampling_sigma_range,
                 std::vector<PriorFunction> priors, std::vector<MarginalizationFunction> marginalization_func_list,
                 std::vector<std::shared_ptr<PhzLikelihood::ProcessModelGridFunctor>> model_funct_list,
                 bool                                                                 doNormalizePdf);

  /**
   * Iterates through a set of sources and calculates the PHZ parameters for
   * each of them. The CatalogHandler makes the assumption that all the sources
   * contain the Photometry attribute. The output is provided by calls to the
   * given OutputHandler. There is one call performed for each source.
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
   */
  template <typename SourceIter>
  void handleSources(SourceIter source_begin, SourceIter source_end, PhzOutput::OutputHandler& out_handler,
                     ProgressListener progress_listener = ProgressListener{}) const;

private:
  SourcePhzFunctor m_source_phz_func;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#include "PhzLikelihood/_impl/CatalogHandler.icpp"

#endif /* PHZLIKELIHOOD_CATALOGHANDLER_H */
