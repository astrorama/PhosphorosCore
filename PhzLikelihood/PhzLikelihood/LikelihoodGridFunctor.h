/**
 * @file LikelihoodGridFunctor.h
 * @date May 15, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_LIKELIHOODGRIDFUNCTOR_H
#define	PHZLIKELIHOOD_LIKELIHOODGRIDFUNCTOR_H

#include <tuple>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class LikelihoodGridFunctor
 *
 * @brief
 * Calculates the likelihood grid of a source over a model photometry grid
 *
 * @details
 * The computed likelihood grids are normalized in such way, so their pick value
 * always has the value 1.
 */
class LikelihoodGridFunctor {

public:

  /**
   * The result type of the LikelihoodGridFunctor. It contains the following:
   * - A grid containing the Likelihood of each model, normalized so the maximum
   *   likelihood is 1
   * - A grid containing the scale factor of each model
   * - A double with the smallest value of the chi square (before normalization)
   */
  using result_type = std::tuple<PhzDataModel::LikelihoodGrid,
                                 PhzDataModel::ScaleFactordGrid,
                                 double>;

  /**
   * Definition of the STL-like algorithm for calculating the grid containing
   * the natural logarithm of the likelihood. It is a function which gets a
   * source photometry and an iterator over the model photometries and populates
   * the likelihood logarithm and scale factor results, using a likelihood
   * iterator and a scale factor iterator.
   */
  typedef std::function<void(const SourceCatalog::Photometry& source_photometry,
                             PhzDataModel::PhotometryGrid::const_iterator model_begin,
                             PhzDataModel::PhotometryGrid::const_iterator model_end,
                             PhzDataModel::LikelihoodGrid::iterator likelihood_log_begin,
                             PhzDataModel::ScaleFactordGrid::iterator scale_factor_begin)
                       > LikelihoodLogarithmFunction;

  /**
   * Constructs a new LikelihoodGridFunctor instance.
   */
  LikelihoodGridFunctor(LikelihoodLogarithmFunction likelihood_log_func);

  /**
   * Computes the likelihood of the given source photometry over the given
   * photometry grid.
   *
   * @param source_phot
   *    The photometry of the source
   * @param phot_grid
   *    The grid containing the model photometries
   * @return
   *    The results of the likelihood calculation
   * \see result_type
   */
  result_type operator()(const SourceCatalog::Photometry& source_phot,
                         const PhzDataModel::PhotometryGrid& phot_grid);

private:

  LikelihoodLogarithmFunction m_likelihood_log_func;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_LIKELIHOODGRIDFUNCTOR_H */

