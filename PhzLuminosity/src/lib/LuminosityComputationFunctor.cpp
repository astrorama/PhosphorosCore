/*
 * LuminosityComputationFunctor.cpp
 *
 *  Created on: Aug 3, 2015
 *      Author: fdubath
 */

#include "PhzLuminosity/LuminosityComputationFunctor.h"

namespace Euclid {
namespace PhzLuminosity {


LuminosityComputationFunctor::LuminosityComputationFunctor(XYDataset::QualifiedName luminosity_filter):
    m_luminosity_filter(luminosity_filter){
}


}
}
