/** 
 * @file SingleGridPhzFunctor.h
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H
#define	PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H

#include <vector>
#include <functional>
#include <algorithm>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"

#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"

namespace Euclid {
namespace PhzLikelihood {

class SingleGridPhzFunctor {
  
public:
  
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
  typedef std::function<PhzDataModel::DoubleGrid::iterator(
                              PhzDataModel::DoubleGrid::iterator likelihood_begin,
                              PhzDataModel::DoubleGrid::iterator likelihood_end)
                       > BestFitSearchFunction;

  /**
   * Definition of the function signature for performing the marginalization. It
   * gets as parameter a LikelihoodGrid and it returns a one dimensional grid with only
   * axis the redshift. It should not perform any normalization.
   */
  typedef std::function<PhzDataModel::Pdf1DZ(const PhzDataModel::DoubleGrid&)> MarginalizationFunction;
  
  /**
   * Definition of the function signature for applying a prior to a likelihood grid.
   */
  typedef std::function<void(PhzDataModel::DoubleGrid&,
                             const SourceCatalog::Photometry&,
                             const PhzDataModel::PhotometryGrid&,
                             const PhzDataModel::DoubleGrid&)> PriorFunction;

  /**
   * Constructs a new SingleGridPhzFunctor instance. It gets as parameters the
   * algorithms to use for calculating the likelihood grid, for finding the best
   * fitted model and the algorithm for performing the PDF marginalization. Note
   * that the algorithms can be ommitted which will result to the default chi^2
   * for the likelihood calculation, the maximum likelihood value for the best
   * fitted model and Bayesian for the marginalization.
   *
   * @param priors
   *    The priors to apply to the likelihood
   * @param likelihood_func
   *    The STL-like algorithm for calculating the likelihood grid
   * @param best_fit_search_func
   *    The STL-like algorithm for finding the best fitted model
   * @param marginalization_func
   *    The functor to use for performing the PDF marginalization
   */
  SingleGridPhzFunctor(std::vector<PriorFunction> priors = {},
                       MarginalizationFunction marginalization_func = BayesianMarginalizationFunctor{},
                       LikelihoodGridFunction likelihood_func = LikelihoodGridFunctor{LikelihoodLogarithmAlgorithm{ScaleFactorFunctorSimple{}, ChiSquareLikelihoodLogarithmSimple{}}},
                       BestFitSearchFunction best_fit_search_func = std::max_element<PhzDataModel::DoubleGrid::iterator>);

  /**
   * Calculates the PHZ results for a single model grid of the parameter space.
   * The given results object must already have set the following:
   * - MODEL_GRID_REFERENCE
   * - SOURCE_PHOTOMETRY_REFERENCE
   * - FIXED_REDSHIFT (if the redshift should be fixed)
   * 
   * The likelihood grid is calculated by using the likelihood algorithm (as
   * given at the constructor). Then, all the priors are applied for computing
   * the posterior. The best matched model is computed based on the posterior.
   * Then, the posterior is used for computing the 1D PDF of the redshift.
   * 
   * The SingleGridPhzFunctor does not directly add results to the given object,
   * but it validates that the functions it used have added the following
   * results:
   * - BEST_MODEL_ITERATOR
   * - SCALE_FACTOR_GRID
   * - LIKELIHOOD_GRID
   * - POSTERIOR_GRID
   * - Z_1D_PDF
   * - Z_1D_PDF_NORM_LOG
   *
   * @param results
   *    The RegionResults object to add the results
   */
  void operator()(PhzDataModel::RegionResults& results) const;
  
private:
  
  std::vector<PriorFunction> m_priors;
  MarginalizationFunction m_marginalization_func;
  LikelihoodGridFunction m_likelihood_func;
  BestFitSearchFunction m_best_fit_search_func;
  
  /// This is a helper method to avoid creating a copy of the grid when we have
  /// fixed redshift
  void computeEverything(const PhzDataModel::PhotometryGrid& phot_grid,
                         PhzDataModel::RegionResults& results) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H */

