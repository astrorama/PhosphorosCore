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
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class LikelihoodGridFunctor
 *
 * @brief
 * Calculates the grid with the likelihood logarithm of a source over a model
 * photometry grid
 */
class LikelihoodGridFunctor {

public:

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
                             PhzDataModel::DoubleGrid::iterator likelihood_log_begin,
                             PhzDataModel::DoubleGrid::iterator scale_factor_begin)
                       > LikelihoodLogarithmFunction;

  /**
   * Constructs a new LikelihoodGridFunctor instance.
   */
  LikelihoodGridFunctor(LikelihoodLogarithmFunction likelihood_log_func);

  /**
   * Computes the log likelihood of the given source photometry over the given
   * photometry grid. The given results object must already contain the
   * MODEL_GRID_REFERENCE and SOURCE_PHOTOMETRY_REFERENCE objects. After the
   * call, the results will contain the LIKELIHOOD_GRID, SCALE_FACTOR_GRID
   * and SAMPLE_SCALE_FACTOR (set to false)
   *
   * @param results
   *    The results object to get the input and set the output
   */
  void operator()(PhzDataModel::RegionResults& results);

private:

  LikelihoodLogarithmFunction m_likelihood_log_func;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_LIKELIHOODGRIDFUNCTOR_H */

