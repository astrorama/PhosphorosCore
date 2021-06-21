/**
 * @file PhzModeling/NormalizationFunctor.h
 * @date 23/03/2021
 * @author dubathf
 */

#ifndef PHZMODELING_NORMALIZATIONFUNCTOR_H
#define	PHZMODELING_NORMALIZATIONFUNCTOR_H

#include <memory>
#include <cmath>
#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"

namespace Euclid {

namespace XYDataset {
  class XYDataset;
}
namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::NormalizationFunctor
 * @brief
 * This functor is in charge of normalizing the SED
 * @details
 * This functor is applying normalization to the SED so that the
 * absolute luminosity in the selected filter is 1 solar lum.
 *
 */
class NormalizationFunctor {

public:

  /**
   * @brief Constructor
   * @details
   * Store the specific filter and normalization value into the functor
   */
  NormalizationFunctor(PhzDataModel::FilterInfo filter_info, double integrated_flux);

  /**
  * @brief Function Call Operator
  * @details
  * Apply Normalization on the SED
  *
  * @param sed
  * A XYDataset representing the SED to be normalized.
  *
  * @return
  * A XYDataset representing the normalized SED.
  */
  Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed) const;

  double getReferenceFlux() const;

private:
  std::vector<PhzDataModel::FilterInfo> m_filter_info;
  double m_integrated_flux;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_NORMALIZATIONFUNCTOR_H */

