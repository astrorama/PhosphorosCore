/**
 * @file PhzLuminosity/UnreddenedLuminosityCalculator.h
 * @date August 19, 2015
 * @author Florian dubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_

#include "XYDataset/QualifiedName.h"
#include "PhzLuminosity/LuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {
/**
 * @class Euclid::PhzLuminosity::UnreddenedLuminosityCalculator
 *
 * @brief Compute the luminosity a source would have (assuming it match a given model)
 * if it was placed at 10[pc]. and has no intrinsic absorption (reddening).
 */
class UnreddenedLuminosityCalculator: public LuminosityCalculator {
public:
  /**
   * @brief constructor
   *
   * @param luminosity_filter
   * The name of the filter the luminosity is computed for.
   *
   * @param model_photometry_grid
   * The PhotometryGrid containing the Luminosity photometry of
   * the (un-reshifted) models
   *
   * @param luminosity_distance_map a map between the redshift and the
   * Luminosity distance
   *
   * @param distance_modulus_map a map between the redshift and the
   * Luminosity distance
   *
   * @param in_mag
   * Define if the luminosity is requiered in Magnitude (true) or flux
   */
  UnreddenedLuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      std::map<double, double> luminosity_distance_map,
      std::map<double, double> distance_modulus_map,
      bool in_mag = true);
  /**
   * @brief destructor
   */
  virtual ~UnreddenedLuminosityCalculator() = default;


  /**
   * @brief Select the Luminosity Model Grid iterator based on the scale_factor
   * iterator. The Axis are fixed to the same SED, and Reddening curve while
   * the redshift and E(B-V) are set to 0.
   *
   * @param scale_factor
   * An iterator on the scall factor grid allowing to gain access on the grid
   * coordinate and to the scalefactor of this specific model (with respect to the source)
   *
   * @param sed
   * SED coordinate of the model. Provided to avoid getting it out of the iterator.
   *
   * @return the iterator on the luminosity model the luminosity has to be computed for.
   */
  const PhzDataModel::PhotometryGrid::const_iterator fixIterator(
         const PhzDataModel::DoubleGrid::const_iterator& scale_factor) const override;

};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_UNREDDENEDLUMINOSITYCALCULATOR_H_ */
