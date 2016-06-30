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
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"
#include "PhzOutput/OutputHandler.h"
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
  typedef std::function<PhzDataModel::LikelihoodGrid::iterator(
                              PhzDataModel::LikelihoodGrid::iterator likelihood_begin,
                              PhzDataModel::LikelihoodGrid::iterator likelihood_end)
                       > BestFitSearchFunction;

  /**
   * Definition of the function signature for performing the marginalization. It
   * gets as parameter a LikelihoodGrid and it returns a one dimensional grid with only
   * axis the redshift. It should not perform any normalization.
   */
  typedef std::function<PhzDataModel::Pdf1DZ(const PhzDataModel::LikelihoodGrid&)> MarginalizationFunction;
  
  /**
   * Definition of the function signature for applying a prior to a likelihood grid.
   */
  typedef std::function<void(PhzDataModel::LikelihoodGrid&,
                             const SourceCatalog::Photometry&,
                             const PhzDataModel::PhotometryGrid&,
                             const PhzDataModel::ScaleFactordGrid&)> PriorFunction;

  /**
   * Constructs a new SingleGridPhzFunctor instance. It gets as parameters a
   * const reference to the grid with the model photometries, the algorithm to
   * use for calculating the likelihood grid, the algorithm for finding the best
   * fitted model and the algorithm for performing the PDF marginalization. Note
   * that the algorithms can be ommitted which will result to the default chi^2
   * for the likelihood calculation, the maximum likelihood value for the best
   * fitted model and Bayesian for the marginalization.
   *
   * @param region_name
   *    The name of the parameter space region the given model grid represents
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
  SingleGridPhzFunctor(const std::string& region_name,
                       const PhzDataModel::PhotometryGrid& phot_grid,
                       std::vector<PriorFunction> priors = {},
                       MarginalizationFunction marginalization_func = BayesianMarginalizationFunctor{},
                       LikelihoodGridFunction likelihood_func = LikelihoodGridFunctor{LikelihoodLogarithmAlgorithm{ScaleFactorFunctorSimple{}, ChiSquareLikelihoodLogarithmSimple{}}},
                       BestFitSearchFunction best_fit_search_func = std::max_element<PhzDataModel::LikelihoodGrid::iterator>);

  /**
   * Calculates the PHZ results for the given source photometry. The likelihood
   * grid is calculated by using the likelihood algorithm (as given at the
   * constructor). The results are added to the given SourceResults object (result
   * types starting with REGION_).
   *
   * @param source_phot
   *    The photometry of the source
   * @param results
   *    The SourceResults object to add the results for the specific region
   * @param fixed_z
   *    If not negative, the redshift will be fixed to the given value
   */
  void operator()(const SourceCatalog::Photometry& source_phot,
                  PhzDataModel::SourceResults& results, double fixed_z=-99) const;
  
private:
  
  std::string m_region_name;
  const PhzDataModel::PhotometryGrid& m_phot_grid;
  std::vector<PriorFunction> m_priors;
  MarginalizationFunction m_marginalization_func;
  LikelihoodGridFunction m_likelihood_func;
  BestFitSearchFunction m_best_fit_search_func;
  
  /// This is a helper method to avoid creating a copy of the grid when we have
  /// fixed redshift
  void computeEverything(const SourceCatalog::Photometry& source_phot,
                         const PhzDataModel::PhotometryGrid& phot_grid,
                         PhzDataModel::SourceResults& results) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SINGLEGRIDPHZFUNCTOR_H */

