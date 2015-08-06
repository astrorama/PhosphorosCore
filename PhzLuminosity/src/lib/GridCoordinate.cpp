/*
 * GridCoordinate.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */


#include "PhzLuminosity/GridCoordinate.h"

namespace Euclid {
namespace PhzLuminosity {

GridCoordinate::GridCoordinate(double new_z, double new_ebv, XYDataset::QualifiedName new_reddening_curve, XYDataset::QualifiedName new_sed):
    z{new_z},ebv{new_ebv}, reddening_curve{std::move(new_reddening_curve)}, sed{std::move(new_sed)}{}


}
}

