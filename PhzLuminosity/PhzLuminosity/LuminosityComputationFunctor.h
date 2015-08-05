/*
 * LuminosityComputationFunctor.h
 *
 *  Created on: Aug 3, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCOMPUTATIONFUNCTOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCOMPUTATIONFUNCTOR_H_

#include <cmath>
#include "ElementsKernel/Exception.h"
#include "XYDataset/QualifiedName.h"
#include "PhysicsUtils/Cosmology.h"
#include "PhzDataModel/PhzModel.h"


namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::GridCoordinate
 *
 * @brief Structure used to store the coordinate in the parameter space of a given model.
 */
struct GridCoordinate{
  GridCoordinate(double new_z, double new_ebv, XYDataset::QualifiedName new_reddening_curve, XYDataset::QualifiedName new_sed);

  virtual ~GridCoordinate() = default;

  double z;
  double ebv;
  XYDataset::QualifiedName reddening_curve;
  XYDataset::QualifiedName sed;
};


/**
 * @class Euclid::PhzLuminosity::LuminosityComputationFunctor
 *
 * @brief Functor computing the absolute magnitude, in a specified filter, of a given
 * model placed at redshift z=0 and scalled according to the scale factor
 *
 */
class LuminosityComputationFunctor {

public:
  /**
   * @brief Constructor
   *
   * @param luminosity_filter
   * The XYDataset::QualifiedName of the filter the absolute magnitude has to be computed for.
   */
  LuminosityComputationFunctor(XYDataset::QualifiedName luminosity_filter);


  /**
   * @brief  Function Call Operator
   * @details
   * Run the computation of the absolute magnitude for the luminosity_filter,
   * using the scaleFactor to scale the model (picked for z=0 into the modelPhotometryGrid)
   *
   * @param gridCoordinate
   * The point into the parameter space the magnitude has to be computed for.
   *
   * @param scaleFactor
   * The (precomputed) scaling of the model (for matching the source photometries).
   *
   * @param modelPhotometryGrid
   * The grid containing the Model photometry.
   * WARNING: this grid must contains a full slice at z=0
   * of the parameter space and has the photometry defined for the luminosity_filter.
   *
   */
  template< typename ScaleFactor, typename ModelPhotometryGrid, typename SourcePhotometry>
  double operator()(const GridCoordinate& gridCoordinate,
      const ScaleFactor& scaleFactor,
      const SourcePhotometry&,
      const ModelPhotometryGrid & modelPhotometryGrid);

  virtual ~LuminosityComputationFunctor() = default;

private:
  XYDataset::QualifiedName m_luminosity_filter;
  PhysicsUtils::Cosmology m_cosmology{};

};

}
}


#include "PhzLuminosity/_impl/LuminosityComputationFunctor.icpp"

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYCOMPUTATIONFUNCTOR_H_ */
