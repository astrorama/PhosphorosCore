/*
 * LuminosityCalculator.h
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#include <memory>
#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"

namespace Euclid {
namespace PhzLuminosity {
/**
 * @class Euclid::PhzLuminosity::LuminosityCalculator
 *
 * @brief The interface for the computation of the luminosity based on
 * the scaling of the photometry of the luminosity filter by the scale factor.
 */
class LuminosityCalculator{
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
  LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      bool in_mag=true);

  /**
   * @brief destructor
   */
  virtual ~LuminosityCalculator() = default;

  /**
   * @brief Clone the calculator.
   */
  virtual std::unique_ptr<LuminosityCalculator> clone() const=0;

  /**
   * @brief Compute the luminosity for the source assuming it match the model
   * (which parameter space coordinate are the same as the scale factor iterator)
   * scaled with the scale factor.
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
  virtual double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
        const double& z,
        const XYDataset::QualifiedName& sed) const=0;

  /**
   * @brief Methode factorizing the computation of the luminosity once the model
   * containing the right photometry for the luminosity filter has been selected.
   *
   */
  double getLuminosityFromModel(
        const PhzDataModel::PhotometryGrid::const_iterator& model,
        double scaleFactor,
        double z) const;

protected:
  /**
   * @brief facade over the call to the cosmology allowing to use a cache
   */
  double getLuminosityDistance(double z) const;

  /**
   * @brief facade over the call to the cosmology allowing to use a cache
   */
  double getDistanceModulus(double z) const;

  XYDataset::QualifiedName m_luminosity_filter;
  std::shared_ptr<PhzDataModel::PhotometryGrid> m_model_photometry_grid;
  bool m_in_mag;
  PhysicsUtils::Cosmology m_cosmology{};

};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_ */
