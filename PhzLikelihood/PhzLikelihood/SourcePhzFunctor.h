/**
 * @file SourcePhzFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H
#define	PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H

#include <algorithm>
#include <functional>
#include <tuple>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "XYDataset/XYDataset.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class SourcePhzFunctor
 *
 * @brief Class responsible for producing the PHZ outputs for a single source
 *
 * @details
 * This class operates as a functor, which recieves a source photometry and
 * returns the PHZ results.
 *
 * The calculation of the PHZ results is performed in the following way. First,
 * the photometric correction is applied to the given source photometry. Then,
 * a grid containing the model photometries is used for creating a likelihood
 * grid of the corrected source photometry. This likelihood grid is used for
 * finding the best fitted model and for calculating the 1D PDF.
 */
class SourcePhzFunctor {

public:

  typedef PhzOutput::OutputHandler::result_type result_type;
  
  /**
   * Definition of the functor for calculating the likelihood grid. It is a
   * function which gets a source photometry and a grid with model photometries,
   * and returns the likelihood grid, the scale factor grid and the minimum
   * chi square value.
   */
  typedef std::function<LikelihoodGridFunctor::result_type(
                                const SourceCatalog::Photometry& source_phot,      
                                const PhzDataModel::PhotometryGrid& phot_grid)
                       > LikelihoodGridFunction;

  /**
   * Definition of the STL-like algorithm for finding the best fitted model. It
   * gets as parameters the iterator over the likelihood grid and it returns an
   * iterator pointing to the best fitted model.
   */
  typedef std::function<PhzDataModel::LikelihoodGrid::iterator(
                              PhzDataModel::LikelihoodGrid::iterator likelihood_begin,
                              PhzDataModel::LikelihoodGrid::iterator likelihood_end)
                       > BestFitSearchFunction;

  /**
   * Definition of the function signature for performing the marginalization. It
   * gets as parameter a LikelihoodGrid and it returns a one dimensional grid with only
   * axis the redshift.
   */
  typedef std::function<PhzDataModel::Pdf1D(const PhzDataModel::LikelihoodGrid&)> MarginalizationFunction;
  
  /**
   * Definition of the function signature for applying a prior to a likelihood grid.
   */
  typedef std::function<void(PhzDataModel::LikelihoodGrid&,
                             const SourceCatalog::Photometry&,
                             const PhzDataModel::PhotometryGrid&,
                             const PhzDataModel::ScaleFactordGrid&)> PriorFunction;

  /**
   * Constructs a new SourcePhzFunctor instance. It gets as parameters a map
   * containing the photometric corrections, a const reference to the grid with
   * the model photometries, the algorithm to use for calculating the likelihood
   * grid, the algorithm for finding the best fitted model and the algorithm for
   * performing the PDF marginalization. Note that the algorithms can be
   * ommitted which will result to the default chi^2 for the likelihood
   * calculation, the maximum likelihood value for the best fitted model and
   * simple summing for the marginalization.
   *
   * @param phot_corr_map
   *    The map with the photometric corrections
   * @param phot_grid
   *    The const reference to the grid with the model photometries
   * @param priors
   *    The priors to apply to the likelihood
   * @param likelihood_func
   *    The STL-like algorithm for calculating the likelihood grid
   * @param best_fit_search_func
   *    The STL-like algorithm for finding the best fitted model
   * @param marginalization_func
   *    The functor to use for performing the PDF marginalization
   */
  SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                   const PhzDataModel::PhotometryGrid& phot_grid,
                   std::vector<PriorFunction> priors = {},
                   MarginalizationFunction marginalization_func = SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{},
                   LikelihoodGridFunction likelihood_func = LikelihoodGridFunctor{},
                   BestFitSearchFunction best_fit_search_func = std::max_element<PhzDataModel::LikelihoodGrid::iterator>);

  /**
   * Calculates the PHZ results for the given source photometry. The given
   * photometry is first being corrected by using the photometric corrections
   * and then its likelihood grid is calculated by using the likelihood algorithm
   * (as given at the constructor). The order of the filters of the source
   * photometry does not matter, but the SourcePhzFunctor instance must contain
   * photometric corrections for all of them, otherwise an ElementsException
   * will be thrown. The second requirement is that the source photometry must
   * contain information for all the filters of the model photometries.
   *
   * @param source_phot
   *    The photometry of the source
   * @return
   *    The PHZ results for the given source
   * @throws ElementsException
   *    if the source photometry contains filters for which the SourcePhzFunctor
   *    has no photometric corrections
   * @throws ElementsException
   *    if the source photometry does not contain information for all the filters
   *    of the model photometries
   */
  result_type operator()(const SourceCatalog::Photometry& source_phot) const;

private:

  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const PhzDataModel::PhotometryGrid& m_phot_grid;
  std::vector<PriorFunction> m_priors;
  MarginalizationFunction m_marginalization_func;
  LikelihoodGridFunction m_likelihood_func;
  BestFitSearchFunction m_best_fit_search_func;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H */

