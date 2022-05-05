/**
 * Copyright (C) 2022 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/** 
 * @file IntegrateDatasetFunctor.h
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_INTEGRATEDATASETFUNCTOR_H
#define	PHZMODELING_INTEGRATEDATASETFUNCTOR_H

#include <utility>
#include "MathUtils/interpolation/interpolation.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class IntegrateDatasetFunctor
 * 
 * @brief
 * Functor responsible for calculating the integral of a function represented by
 * an XYDataset::XYDataset points
 * 
 * @details
 * This class performs the integration by using interpolation. The type of the
 * interpolation can be controlled with the parameters of the constructor.
 */
class IntegrateDatasetFunctor {
  
public:
  
  /**
   * @brief Constructs a new IntegrateDatasetFunctor instance, which will use the
   * given type of interpolation
   * @param type The type of interpolation to use
   */
  IntegrateDatasetFunctor(MathUtils::InterpolationType type);
  
  /**
   * @brief Returns the integral of the given dataset in the given range
   * @details
   * The function outside the dataset range is assumed to be zero.
   * @param dataset The dataset to integrate
   * @param range The range to integrate
   * @return The integral value
   */
  double operator()(const XYDataset::XYDataset& dataset, const std::pair<double, double>& range) const;
  
private:
  
  MathUtils::InterpolationType m_type;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_INTEGRATEDATASETFUNCTOR_H */

