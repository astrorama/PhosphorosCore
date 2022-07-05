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
 * @file PhzLikelihood/GalacticAbsorptionProcessModelGridFunctor.h
 * @date 11/28/18
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_GALACTICABSORPTIONPROCESSMODELGRIDFUNCTOR_H
#define PHZLIKELIHOOD_GALACTICABSORPTIONPROCESSMODELGRIDFUNCTOR_H

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzLikelihood/ProcessModelGridFunctor.h"
#include "SourceCatalog/Source.h"
#include <map>
#include <string>

namespace Euclid {
namespace PhzLikelihood {

class GalacticAbsorptionProcessModelGridFunctor : public ProcessModelGridFunctor {
public:
  GalacticAbsorptionProcessModelGridFunctor(const std::map<std::string, PhzDataModel::PhotometryGrid>& coefficient_grid,
                                            double dust_map_sed_bpc);
  ~GalacticAbsorptionProcessModelGridFunctor(){};

  void operator()(const std::string& region_name, const SourceCatalog::Source& source,
                  PhzDataModel::PhotometryGrid& model_grid) const override;

protected:
  const std::map<std::string, PhzDataModel::PhotometryGrid>& m_coefficient_grid;

private:
  double m_dust_map_sed_bpc;
};

}  // end of namespace PhzLikelihood
}  // end of namespace Euclid

#endif /* PHZLIKELIHOOD_GALACTICABSORPTIONPROCESSMODELGRIDFUNCTOR_H */
