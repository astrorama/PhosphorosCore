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
 * @file PhzLikelihood/FilterShiftProcessModelGridFunctor.h
 * @date 2021/09/10
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_FILTERSHIFTPROCESSMODELGRIDFUNCTOR_H
#define PHZLIKELIHOOD_FILTERSHIFTPROCESSMODELGRIDFUNCTOR_H

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "SourceCatalog/Source.h"
#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzLikelihood {

class FilterShiftProcessModelGridFunctor : public ProcessModelGridFunctor {
public:
  FilterShiftProcessModelGridFunctor(const std::map<std::string, PhzDataModel::PhotometryGrid>& coefficient_grid);
  ~FilterShiftProcessModelGridFunctor() {}

  void operator()(const std::string& region_name, const SourceCatalog::Source& source,
                  PhzDataModel::PhotometryGrid& model_grid) const override;

  static void computeCorrectedPhotometry(SourceCatalog::Photometry::const_iterator model_begin,
                                         SourceCatalog::Photometry::const_iterator model_end,
                                         SourceCatalog::Photometry::const_iterator corr_begin,
                                         const std::vector<double>&                filter_shift,
                                         SourceCatalog::Photometry::iterator       out_begin);

protected:
  const std::map<std::string, PhzDataModel::PhotometryGrid>& m_coefficient_grid;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#endif /* PHZLIKELIHOOD_FILTERSHIFTPROCESSMODELGRIDFUNCTOR_H */
