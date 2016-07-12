/**
 * @file SourcePhzFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H
#define	PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H

#include <map>
#include <string>
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/SourceResults.h"
#include "PhzLikelihood/SingleGridPhzFunctor.h"

#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"

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
 * the photometric correction is applied to the given source photometry. Then
 * the SingleGridPhzFunctor is used to produce the results for each region of
 * the parameter space. These results are then combined to the final results.
 */
class SourcePhzFunctor {

public:

  using LikelihoodGridFunction = SingleGridPhzFunctor::LikelihoodGridFunction;
  using BestFitSearchFunction = SingleGridPhzFunctor::BestFitSearchFunction;
  using MarginalizationFunction = SingleGridPhzFunctor::MarginalizationFunction;
  using PriorFunction = SingleGridPhzFunctor::PriorFunction;
  
  /**
   * Constructs a new SourcePhzFunctor instance. It gets as parameters a map
   * containing the photometric corrections,a map with the grids with the model
   * photometries for each parameter space region, the algorithm to use for
   * calculating the likelihood grid, the algorithm for finding the best fitted
   * model and the algorithm for performing the PDF marginalization. Note that
   * the algorithms can be omitted which will result to the default chi^2 for
   * the likelihood calculation, the maximum likelihood value for the best
   * fitted model and bayesian for the marginalization.
   *
   * @param phot_corr_map
   *    The map with the photometric corrections
   * @param phot_grid_map
   *    The const reference to the map containing the grids with the model
   *    photometries for all the parameter space regions
   * @param likelihood_grid_func
   *    The function to use for computing the likelihood grid for a single source
   * @param priors
   *    The priors to apply to the likelihood
   * @param best_fit_search_func
   *    The STL-like algorithm for finding the best fitted model
   * @param marginalization_func
   *    The functor to use for performing the PDF marginalization
   */
  SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                   const std::map<std::string, PhzDataModel::PhotometryGrid>& phot_grid_map,
                   LikelihoodGridFunction likelihood_grid_func,
                   std::vector<PriorFunction> priors = {},
                   MarginalizationFunction marginalization_func = BayesianMarginalizationFunctor{},
                   BestFitSearchFunction best_fit_search_func = std::max_element<PhzDataModel::LikelihoodGrid::iterator>);

  /**
   * Calculates the PHZ results for the given source photometry. The given
   * photometry is first being corrected by using the photometric corrections. Then
   * the SingleGridPhzFunctor is used to produce the results for each region of
   * the parameter space. These results are then combined to the final results.
   *
   * @param source_phot
   *    The photometry of the source
   * @param fixed_z
   *    If not negative, the redshift will be fixed to the given value
   * @return
   *    The PHZ results for the given source
   */
  PhzDataModel::SourceResults operator()(const SourceCatalog::Photometry& source_phot, double fixed_z=-99) const;

private:

  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const std::map<std::string, PhzDataModel::PhotometryGrid>& m_phot_grid_map;
  std::map<std::string, SingleGridPhzFunctor> m_single_grid_functor_map {};

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H */

