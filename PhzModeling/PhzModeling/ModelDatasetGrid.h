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
 * PhzModeling/ModelDatasetGrid.h
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#ifndef PHZMODELING_MODELDATASETGRID_H_
#define PHZMODELING_MODELDATASETGRID_H_

#include "XYDataset/XYDataset.h"
#include "PhzModeling/ModelDatasetGenerator.h"

namespace Euclid {
namespace PhzModeling {

struct ModelDatasetCellManager {
  size_t m_size;
};

}
} // end of namespace Euclid

namespace Euclid {
namespace GridContainer {

template<>
struct GridCellManagerTraits<PhzModeling::ModelDatasetCellManager> {
  typedef XYDataset::XYDataset data_type;
  typedef data_type*           pointer_type;
  typedef data_type&           reference_type;

  typedef PhzModeling::ModelDatasetGenerator iterator;
  static std::unique_ptr<PhzModeling::ModelDatasetCellManager> factory(size_t size) {
    return std::unique_ptr<PhzModeling::ModelDatasetCellManager>{
      new PhzModeling::ModelDatasetCellManager {size}
    };
  }
  static size_t begin(const PhzModeling::ModelDatasetCellManager&) {
    return 0;
  }
  static size_t end(const PhzModeling::ModelDatasetCellManager& manager) {
    return manager.m_size;
  }
};

}
} // end of namespace Euclid

namespace Euclid {
namespace PhzModeling {
/**
 * @class PhzModeling::ModelDatasetGrid
 * @brief Specialization of the PhzDataModel::PhzGrid used to store
 * and walk through the parameter space.
 * The grid do not actually store the SED models but compute them dynamically
 * (through the ModelDatsetGenerator).
 */
class ModelDatasetGrid: public PhzDataModel::PhzGrid<ModelDatasetCellManager> {

public:
  
  typedef ModelDatasetGenerator::ReddeningFunction ReddeningFunction;
  
  typedef ModelDatasetGenerator::RedshiftFunction RedshiftFunction;
  
  typedef ModelDatasetGenerator::IgmAbsorptionFunction IgmAbsorptionFunction;
  
  typedef ModelDatasetGenerator::NormalizationFunction NormalizationFunction;

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
  ModelDatasetGrid(const PhzDataModel::ModelAxesTuple& parameter_space,
                   std::map<XYDataset::QualifiedName,XYDataset::XYDataset> sed_map,
                   std::map<XYDataset::QualifiedName,
                     std::unique_ptr<MathUtils::Function> > reddening_curve_map,
                   ReddeningFunction reddening_function,
                   RedshiftFunction redshift_function,
                   IgmAbsorptionFunction igm_function,
                   NormalizationFunction normalization_function);

  /**
  * @brief begin function for the iteration.
  */
  PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator begin();

  /**
  * @brief end function for the iteration.
  */
  PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator end();

private:
  size_t m_size;

  std::map<XYDataset::QualifiedName,XYDataset::XYDataset> m_sed_map;
  std::map<XYDataset::QualifiedName,std::unique_ptr<MathUtils::Function> > m_reddening_curve_map;
  ReddeningFunction m_reddening_function;
  RedshiftFunction m_redshift_function;
  IgmAbsorptionFunction m_igm_function;
  NormalizationFunction m_normalization_function;
};

}
}

#endif /* PHZMODELING_MODELDATASETGRID_H_ */
