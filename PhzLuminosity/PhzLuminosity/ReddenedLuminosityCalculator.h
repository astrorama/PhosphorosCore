/*
 * ReddenedLuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_

#include "XYDataset/QualifiedName.h"
#include "PhzLuminosity/LuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::ReddenedLuminosityCalculator
 *
 * @brief Compute the luminosity a source would have (assuming it match a given model)
 * if it was placed at 10[pc].
 */
class ReddenedLuminosityCalculator: public LuminosityCalculator {
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
   * @param in_mag
   * Define if the luminosity is requiered in Magnitude (true) or flux
   */
  ReddenedLuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      bool in_mag = true) ;

  /**
   * @brief destructor
   */
  virtual ~ReddenedLuminosityCalculator() = default;

  /**
   * @brief Clone the calculator.
   */
   std::unique_ptr<LuminosityCalculator> clone() const override;

  /**
   * @brief Compute the luminosity for the source assuming it match the model
   * (which parameter space coordinate are the same as the scale factor iterator)
   * scaled with the scale factor if it was placed at 10[pc].
   * Note that the redshift and SED coordinate are redundant but provided
   * for optimization purpose.
   *
   * @param scale_factor
   * An iterator on the scall factor grid allowing to gain access on the grid
   * coordinate and to the scalefactor of this specific model (with respect to the source)
   *
   * @param z
   * Redshift coordinate of the model. Provided to avoid getting it out of the iterator.
   *
   * @param sed
   * SED coordinate of the model. Provided to avoid getting it out of the iterator.
   */
  double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
      const double& z,
      const XYDataset::QualifiedName& sed) const override;

};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_REDDENEDLUMINOSITYCALCULATOR_H_ */
