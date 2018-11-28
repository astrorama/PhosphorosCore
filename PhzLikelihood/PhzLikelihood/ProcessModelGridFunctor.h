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
 * @file PhzLikelihood/ProcessModelGridFunctor.h
 * @date 11/27/18
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_PROCESSMODELGRIDFUNCTOR_H
#define PHZLIKELIHOOD_PROCESSMODELGRIDFUNCTOR_H

#include <map>
#include <string>
#include "SourceCatalog/Source.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzLikelihood {

class ProcessModelGridFunctor{
  public:
  virtual const PhzDataModel::PhotometryGrid operator()(
      const PhzDataModel::PhotometryGrid & model_grid,
      const SourceCatalog::Source & source) const=0;
  virtual ~ProcessModelGridFunctor(){}
};



} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif  /* PHZLIKELIHOOD_PROCESSMODELGRIDFUNCTOR_H */
