/*
 * ReddenedLuminosityCalculator.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: fdubath
 */


#include "PhzLuminosity/ReddenedLuminosityCalculator.h"

namespace Euclid {
namespace PhzLuminosity {


ReddenedLuminosityCalculator::ReddenedLuminosityCalculator(
    XYDataset::QualifiedName luminosity_filter,
    std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
    bool in_mag)
: LuminosityCalculator(luminosity_filter, model_photometry_grid, in_mag) { }

/**
  * @brief Compute the luminosity for the model (which parameter space
  * coordinate are the same as the scale factor iterator) scaled with the scale factor.
  * The luminosity
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
  double ReddenedLuminosityCalculator::operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
      const double& z,
      const XYDataset::QualifiedName& sed) const{

    auto model_iter = m_model_photometry_grid->cbegin();
    model_iter.fixAxisByIndex<PhzDataModel::ModelParameter::Z>(0);
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(
        scale_factor.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::EBV>(
        scale_factor.axisValue<PhzDataModel::ModelParameter::EBV>());
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    return getLuminosityFromModel(model_iter,*scale_factor,z);

  }


std::unique_ptr<LuminosityCalculator> ReddenedLuminosityCalculator::clone() const{
    return std::unique_ptr<LuminosityCalculator>{new ReddenedLuminosityCalculator(m_luminosity_filter,m_model_photometry_grid,m_in_mag)};
  }

}
}
