/**
 * @file PhotometricCorrectionAlgorithm.h
 * @date January 14, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONALGORITHM_H
#define PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONALGORITHM_H

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzPhotometricCorrection/FindMedianPhotometricCorrectionsFunctor.h"
#include <functional>
#include <map>

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * @class PhotometricCorrectionAlgorithm
 *
 * @brief
 * Calculates the optimal value of the photometric correction for all filters
 */
class PhotometricCorrectionAlgorithm {

public:
  template <typename SourceIter>
  using PhotometricCorrectionSelector = std::function<PhzDataModel::PhotometricCorrectionMap(
      const std::map<typename PhzUtils::SourceIterTraits<SourceIter>::id_type, PhzDataModel::PhotometricCorrectionMap>&
                 source_phot_corr_map,
      SourceIter source_begin, SourceIter source_end)>;

  /**
   * @brief Calculates the photometric corrections so the input sources
   * will best match the given models, scaled by the given factors. All
   * the sources of the catalog and the mapped models are assumed to contain
   * photometries for the same filters. The returned photometric correction is
   * normalized so the median filter has multiplier 1.
   *
   * @tparam SourceIter The type of the iterator over the source objects
   * @tparam ModelPhotPtr A type which can be dereferenced to a Photometry object
   *    representing the model photometry
   *
   * @param source_begin An iterator to the first of the sources
   * @param source_end An iterator to one after the last of the sources
   * @param scale_factor_map The factors to multiply the model photometries for
   *    each source (source ID is the key of the map)
   * @param model_map The objects pointing to the model photometries for each
   *    source (souce ID is the key of the map)
   *
   * @return The calcualted photometric correction
   *
   * @throws ElementsException
   *    if there are no sources in the input
   * @throws ElementsException
   *    if any of the sources does not contain the photometry attribute
   * @throws ElementsException
   *    if the scale factor map does not contain factors for all the sources
   * @throws ElementsException
   *    if the model photometry map does not contain photometries for all the sources
   */
  template <typename SourceIter, typename ModelPhot>
  PhzDataModel::PhotometricCorrectionMap
  operator()(SourceIter source_begin, SourceIter source_end,
             const std::map<typename PhzUtils::SourceIterTraits<SourceIter>::id_type, double>&    scale_factor_map,
             const std::map<typename PhzUtils::SourceIterTraits<SourceIter>::id_type, ModelPhot>& model_phot_map,
             PhotometricCorrectionSelector<SourceIter> selector = FindMedianPhotometricCorrectionsFunctor{}) const;
};

}  // end of namespace PhzPhotometricCorrection
}  // end of namespace Euclid

#include "PhzPhotometricCorrection/_impl/PhotometricCorrectionAlgorithm.icpp"

#endif /* PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONALGORITHM_H */
