/**
 * @file PhzLuminosity/LuminosityCalculator.h
 * @date August 19, 2015
 * @author Florian dubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_
#include <memory>
#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/CosmologicalParameters.h"
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
   * @param luminosity_distance_map a map between the redshift and the
   * Luminosity distance
   *
   * @param distance_modulus_map a map between the redshift and the
   * Luminosity distance
   *
   * @param in_mag
   * Define if the luminosity is requiered in Magnitude (true) or flux
   */
  LuminosityCalculator(XYDataset::QualifiedName luminosity_filter,
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      std::map<double,double> luminosity_distance_map,
      std::map<double,double> distance_modulus_map,
      bool in_mag=true);

  /**
   * @brief destructor
   */
  virtual ~LuminosityCalculator() = default;

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
   *
   * @return The luminosity.
   */
   double operator()(const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
        const double& z,
        const XYDataset::QualifiedName& sed) const;

  /**
   * @brief Select the Luminosity Model Grid iterator based on the scale_factor
   * iterator. For performance purpose the SED is provided so it is not necessary
   * to get it out of the scale_factor iter.
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
   virtual const PhzDataModel::PhotometryGrid::const_iterator fixIterator(
       const PhzDataModel::ScaleFactordGrid::const_iterator& scale_factor,
       const XYDataset::QualifiedName& sed) const =0;

protected:
   std::shared_ptr<PhzDataModel::PhotometryGrid> m_model_photometry_grid;

private:
   XYDataset::QualifiedName m_luminosity_filter;
   mutable std::unique_ptr<std::size_t> m_luminosity_filter_index;
   std::map<double,double> m_luminosity_distance_map;
   std::map<double,double> m_distance_modulus_map;
   bool m_in_mag;

  /**
    * @brief Methode factorizing the computation of the luminosity once the model
    * containing the right photometry for the luminosity filter has been selected.
    *
    */
   double getLuminosityFromModel(const PhzDataModel::PhotometryGrid::const_iterator& model,
                                 double scaleFactor, double z) const;


};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCALCULATOR_H_ */
