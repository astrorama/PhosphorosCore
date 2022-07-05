/**
 * @file SingleGridPhzFunctor.h
 * @date June 2, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H
#define PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/RegionResults.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include <functional>
#include <vector>

#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"
#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

class SingleGridPhzFunctor {

public:
  /**
   * Definition of the functor for calculating the likelihood grid. It is a
   * function which uses the given results objects both for its input and output.
   * When the function is called, the results is guaranteed to contain the
   * following:
   *
   * - MODEL_GRID_REFERENCE
   * - SOURCE_PHOTOMETRY_REFERENCE
   *
   * The given functors must populate the results with the following:
   *
   * - LIKELIHOOD_GRID
   * - SCALE_FACTOR_GRID
   */
  typedef std::function<void(PhzDataModel::RegionResults& results)> LikelihoodGridFunction;

  /**
   * Definition of the function signature for performing the marginalization. It
   * is a function which uses the given result object both for its input and
   * output. When it is called, the passed object is guaranteed to contain the
   * POSTERIOR_GRID result. The different marginalization implementations
   * should produce marginalized versions of this grid, without performing any
   * normalization, and store them to the following results:
   * - SED_1D_PDF
   * - RED_CURVE_1D_PDF
   * - EBV_1D_PDF
   * - Z_1D_PDF
   */
  typedef std::function<void(PhzDataModel::RegionResults& results)> MarginalizationFunction;

  /**
   * Definition of the function signature for applying a prior to the posterior
   * grid. It is a function which uses the given results objects both for its
   * input and output. It requires that the given object already contains the
   * POSTERIOR_GRID result, on which it applies the prior directly. The different
   * prior implementations are free to use also the following results:
   * - MODEL_GRID_REFERENCE
   * - SOURCE_PHOTOMETRY_REFERENCE
   * - SCALE_FACTOR_GRID
   * - LIKELIHOOD_GRID
   * - POSTERIOR_GRID
   */
  typedef std::function<void(PhzDataModel::RegionResults& results)> PriorFunction;

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
   * @param marginalization_func
   *    The functor to use for performing the PDF marginalization
   */
  SingleGridPhzFunctor(std::vector<PriorFunction>           priors = {},
                       std::vector<MarginalizationFunction> marginalization_func_list =
                           {BayesianMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{
                               PhzDataModel::GridType::POSTERIOR}},
                       LikelihoodGridFunction likelihood_func = LikelihoodGridFunctor{LikelihoodLogarithmAlgorithm{
                           ScaleFactorFunctorSimple{}, ChiSquareLikelihoodLogarithmSimple{}}});

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
  std::vector<PriorFunction>           m_priors;
  std::vector<MarginalizationFunction> m_marginalization_func_list;
  LikelihoodGridFunction               m_likelihood_func;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#endif /* PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H */
