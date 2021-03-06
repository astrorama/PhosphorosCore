/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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
 * @file PhzLikelihood/GenericGridPrior.h
 * @date 01/22/16
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_GENERICGRIDPRIOR_H
#define _PHZLIKELIHOOD_GENERICGRIDPRIOR_H

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/RegionResults.h"
#include <vector>

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class GenericGridPrior
 * @brief Class which applies a user defined prior
 * @details
 * This prior can be used only for posterior grids which have the exact same
 * coordinates with the prior grid given by the user. For convenience, the user
 * can give a set of grids to this class, so the class can be reused for many
 * different likelihood grids (in the case of sparse likelihood grid for example).
 */
class GenericGridPrior {

public:
  /// Constructs a new GenericGridPrior, which can handle likelihood grids with
  /// the same axes as the given prior grid list
  GenericGridPrior(std::vector<PhzDataModel::DoubleGrid> prior_grid_list);

  /**
   * @brief Destructor
   */
  virtual ~GenericGridPrior() = default;

  /**
   * @brief Applies the prior to the given posterior grid
   * @details
   * This call assumes that the user gave to the constructor a prior grid with
   * the same axes as the posterior grid. It applies the prior by multiplying
   * all the cells of the posterior grid with the corresponding ones of the
   * prior grid.
   * @param results
   *    The results object containing the posterior to apply the prior to
   * @throws Elements::Exception
   *    If there was no prior grid with the same axes given during construction
   */
  void operator()(PhzDataModel::RegionResults& results) const;

  std::pair<bool, int> checkCompatibility(const PhzDataModel::DoubleGrid& prior,
                                          const PhzDataModel::DoubleGrid& posterior) const;

private:
  std::vector<PhzDataModel::DoubleGrid> m_prior_grid_list;

}; /* End of GenericGridPrior class */

} /* namespace PhzLikelihood */
} /* namespace Euclid */

#endif
