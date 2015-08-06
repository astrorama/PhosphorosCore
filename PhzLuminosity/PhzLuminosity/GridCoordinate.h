/*
 * GridCoordinate.h
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_GRIDCOORDINATE_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_GRIDCOORDINATE_H_


#include "XYDataset/QualifiedName.h"

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

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_GRIDCOORDINATE_H_ */
