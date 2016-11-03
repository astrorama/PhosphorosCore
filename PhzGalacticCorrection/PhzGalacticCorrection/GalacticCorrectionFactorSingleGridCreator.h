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
 * @file PhzGalacticCorrection/GalacticCorrectionSingleGridCreator.h
 * @date 2016/11/02
 * @author Florian Dubath
 */

#ifndef _PHZGALACTICCORRECTION_GALACTICCORRECTIONFACTORSINGLEGRIDCREATOR_H
#define _PHZGALACTICCORRECTION_GALACTICCORRECTIONFACTORSINGLEGRIDCREATOR_H

#include <memory>
#include <tuple>
#include <functional>
#include "MathUtils/function/Function.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "GridContainer/GridAxis.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzModeling/ModelDatasetGrid.h"

namespace Euclid {
namespace PhzGalacticCorrection {

class GalacticCorrectionSingleGridCreator{
public:

  typedef PhzModeling::ModelDatasetGrid::IgmAbsorptionFunction IgmAbsorptionFunction;
  typedef std::function<void(size_t step, size_t total)> ProgressListener;

  GalacticCorrectionSingleGridCreator(
       std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
       std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
       std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
       IgmAbsorptionFunction igm_absorption_function,
       XYDataset::QualifiedName b_filter,
       XYDataset::QualifiedName v_filter,
       XYDataset::QualifiedName milky_way_reddening,
       double dust_sed_bpc
       );

  virtual ~GalacticCorrectionSingleGridCreator();

  PhzDataModel::PhotometryGrid createGrid(PhzDataModel::ModelAxesTuple& parameter_space,
               const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
               ProgressListener progress_listener=ProgressListener{});

private:
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_sed_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_reddening_curve_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  IgmAbsorptionFunction m_igm_absorption_function;
  XYDataset::QualifiedName m_b_filter;
  XYDataset::QualifiedName m_v_filter;
  XYDataset::QualifiedName m_milky_way_reddening;
  double m_dust_sed_bpc;

};


}
}

#endif
