// Copyright (C) 2012-2022 Euclid Science Ground Segment
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

/**
 * @file PhzOutput/PhzColumnHandlers/CorrectedPhotometry.h
 * @date 2022/06/08
 * @author dubathf
 */

#ifndef _PHZOUTPUT_PHZCOLUMNHANDLERS_CORRECTEDPHOTOMETRY_H
#define _PHZOUTPUT_PHZCOLUMNHANDLERS_CORRECTEDPHOTOMETRY_H

#include <string>
#include "PhzOutput/PhzColumnHandlers/ColumnHandler.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {

/**
 * @class CorrectedPhotometry
 * @brief
 *
 */
class CorrectedPhotometry : public ColumnHandler {

public:
  CorrectedPhotometry(const Table::ColumnInfo& column_info,
                      const Configuration::PhotometricBandMappingConfig::MappingMap& mapping,
					  bool do_marginalize,
					  bool correct_filter,
					  bool correct_galactic,
					  double dust_map_sed_bpc,
					  const std::map<std::string, PhzDataModel::PhotometryGrid>& filter_shift_coef_grid,
					  const std::map<std::string, PhzDataModel::PhotometryGrid>& galactic_correction_coef_grid
					 );

  /**
   * @brief Destructor
   */
  virtual ~CorrectedPhotometry() = default;

  std::vector<Table::ColumnInfo::info_type> getColumnInfoList() const override;

  std::vector<Table::Row::cell_type> convertResults(const SourceCatalog::Source&       source,
                                                    const PhzDataModel::SourceResults& sourceResult) const override;

  std::vector<double> computeCorrectionFactorForModel(const SourceCatalog::Source& source,
		                                              size_t region_index,
		                                              const PhzDataModel::PhotometryGrid::const_iterator model) const;

  static std::vector<double> sumVector(double m1, std::vector<double> v1,  double m2, std::vector<double> v2);

  static std::vector<double> multVector(double m1, std::vector<double> v1);

private:
  std::vector<Table::ColumnInfo::info_type> m_columnInfo {};
  bool m_do_marginalize;
  bool m_correct_filter;
  bool m_correct_galactic;
  double m_dust_map_sed_bpc;
  const std::map<std::string, PhzDataModel::PhotometryGrid>& m_filter_shift_coef_grid;
  const std::map<std::string, PhzDataModel::PhotometryGrid>& m_galactic_correction_coef_grid;

  static constexpr double l10 = 2.302585092994046;  // std::log(10.);
}; /* End of Id class */

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */

#endif
