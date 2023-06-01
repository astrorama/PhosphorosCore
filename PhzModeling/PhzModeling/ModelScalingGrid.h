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

/*
 * PhzModeling/ModelScalingGrid.h
 *
 *  Created on: 2023/04/25
 *      Author: fdubath
 */

#ifndef PHZMODELING_MODELSCALINGGRID_H_
#define PHZMODELING_MODELSCALINGGRID_H_

#include "PhzDataModel/PhzModel.h"
#include "PhzModeling/ModelScalingGenerator.h"
#include "XYDataset/XYDataset.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzModeling/NormalizationFunctor.h"

namespace Euclid {
namespace PhzModeling {



}  // namespace PhzModeling
}  // end of namespace Euclid


namespace Euclid {
namespace GridContainer {

template <>
struct GridCellManagerTraits<PhzDataModel::DoubleCellManager> {
  typedef double data_type;
  typedef double*           pointer_type;
  typedef double&           reference_type;

  typedef PhzModeling::ModelScalingGenerator                   iterator;
  static std::unique_ptr<PhzDataModel::DoubleCellManager> factory(size_t size) {
    return std::unique_ptr<PhzDataModel::DoubleCellManager>{new PhzDataModel::DoubleCellManager(size)};
  }
  static size_t begin(const PhzDataModel::DoubleCellManager&) {
    return 0;
  }
  static size_t end(const PhzDataModel::DoubleCellManager& manager) {
    return manager.size();
  }
};

}  // namespace GridContainer
}  // end of namespace Euclid


namespace Euclid {
namespace PhzModeling {
/**
 * @class PhzModeling::ModelScalingGrid
 * @brief Specialization of the PhzDataModel::PhzGrid used to store
 * and walk through the parameter space.
 * The grid do not actually store the SED models but compute them dynamically
 * (through the ModelScalingGenerator).
 */
class ModelScalingGrid : public PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager> {

public:
  typedef ModelScalingGenerator::ReddeningFunction ReddeningFunction;


  /**
   * @brief Constructor
   * @details
   * Constructor.
   *
   * @param parameter_space
   * A ModelAxesTuple defining the axes of the the Grid.
   *
   * @param sed_map
   * A map storing the SED template and giving access to them through
   * the QualifiedName.
   * As the XYDataset cannot be copied one must move this argument in.
   *
   * @param reddening_curve_map
   * A map storing the extinction curve as a Function and giving access to them
   * through the QualifiedName.
   * As the unique_ptr cannot be copied one must move this argument in.
   *
   * @param reddening_function
   * A function used to apply the extinction to a SED
   *
   * @param redshift_function
   * A function used to apply the redshit to a SED
   *
   * @param igm_function
   * A function used to apply the IGM absorption to a redshifted SED
   */
  ModelScalingGrid(const PhzDataModel::ModelAxesTuple&                                      parameter_space,
                   std::map<XYDataset::QualifiedName, XYDataset::XYDataset>                 sed_map,
                   std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> reddening_curve_map,
                   ReddeningFunction                                                        reddening_function,
				   NormalizationFunctor                                                     normalization_functor);

  /**
   * @brief begin function for the iteration.
   */
  PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator begin();

  /**
   * @brief end function for the iteration.
   */
  PhzDataModel::PhzGrid<PhzDataModel::DoubleCellManager>::iterator end();

private:
  size_t m_size;

  std::map<XYDataset::QualifiedName, XYDataset::XYDataset>                 m_sed_map;
  std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> m_reddening_curve_map;
  ReddeningFunction                                                        m_reddening_function;
  NormalizationFunctor                                                     m_normalization_functor;
};

}  // namespace PhzModeling
}  // namespace Euclid

#endif /* PHZMODELING_MODELSCALINGGRID_H_ */
