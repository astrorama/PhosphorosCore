/**
 * @file ScalingSamplingLikelihoodGridFunctor.h
 * @date 24/02/2021
 * @author Dubathf
 */

#ifndef PHZLIKELIHOOD_SCALINGSAMPLINGLIKELIHOODGRIDFUNCTOR_H
#define	PHZLIKELIHOOD_SCALINGSAMPLINGLIKELIHOODGRIDFUNCTOR_H

#include <tuple>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/DoubleListGrid.h"
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
class ScalingSamplingLikelihoodGridFunctor {

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
                             PhzDataModel::DoubleGrid::iterator scale_factor_begin,
                             PhzDataModel::DoubleGrid::iterator sigma_scale_factor_begin,
                             PhzDataModel::DoubleListGrid::iterator likelihood_log_sample_begin
                             )
                       > LikelihoodScaleSampleLogarithmFunction;

  /**
   * Constructs a new LikelihoodGridFunctor instance.
   */
  ScalingSamplingLikelihoodGridFunctor(LikelihoodScaleSampleLogarithmFunction likelihood_scale_sample_log_func);

  /**
   * Computes the log likelihood of the given source photometry over the given
   * photometry grid. The given results object must already contain the
   * MODEL_GRID_REFERENCE and SOURCE_PHOTOMETRY_REFERENCE objects. After the
   * call, the results will contain the LIKELIHOOD_GRID and SCALE_FACTOR_GRID,
   * SIGMA_SCALE_FACTOR_GRID,  LIKELIHOOD_LOG_SCALING_GRID
   * and SAMPLE_SCALE_FACTOR (set to true) .
   *
   * @param results
   *    The results object to get the input and set the output
   */
  void operator()(PhzDataModel::RegionResults& results);

private:

  LikelihoodScaleSampleLogarithmFunction m_likelihood_scale_sample_log_func;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SCALINGSAMPLINGLIKELIHOODGRIDFUNCTOR_H */

