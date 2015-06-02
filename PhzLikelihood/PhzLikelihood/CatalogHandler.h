/** 
 * @file CatalogHandler.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_CATALOGHANDLER_H
#define	PHZLIKELIHOOD_CATALOGHANDLER_H

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

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
  
  /**
   * Constructs a new CatalogHandler instance. If the given photometric
   * correction map does not contain corrections for all the filters of the
   * given model photometries an exception is thrown.
   * 
   * @param phot_corr_map
   *    A map with the photometric corrections to be applied for each filter
   * @param phot_grid_map
   *    The const reference to the map containing the grids with the model
   *    photometries for all the parameter space regions
   * @param priors
   *    The priors to apply on the likelihood grid
   * @param marginalization_func
   *    The function to use for marginalizing the multi-dimensional likelihood
   *    grid to a 1D PDF
   * @throws ElementsException
   *    If the phot_corr_map does not contain photometric corrections for all
   *    the filters of the model photometries
   */
  CatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                 const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                 std::vector<PriorFunction> priors,
                 MarginalizationFunction marginalization_func);
  
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
  template<typename SourceIter>
  void handleSources(SourceIter source_begin, SourceIter source_end,
                     PhzOutput::OutputHandler& out_handler) const;
  
private:
  
  SourcePhzFunctor m_source_phz_func;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/CatalogHandler.icpp"

#endif	/* PHZLIKELIHOOD_CATALOGHANDLER_H */

