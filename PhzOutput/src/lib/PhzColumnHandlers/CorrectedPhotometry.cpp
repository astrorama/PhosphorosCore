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

/* 
 * @file CorrectedPhotometry.cpp
 * @author dubathf
 */
#include <cstdlib>
#include "ElementsKernel/Logging.h"

#include "PhzDataModel/PhotometryGrid.h"
#include "SourceCatalog/SourceAttributes/TableRowAttribute.h"
#include "PhzOutput/PhzColumnHandlers/CorrectedPhotometry.h"
#include "PhzDataModel/CatalogAttributes/ObservationCondition.h"

namespace Euclid {
namespace PhzOutput {
namespace ColumnHandlers {


static Elements::Logging logger = Elements::Logging::getLogger("CorrectedPhotometry");

CorrectedPhotometry::CorrectedPhotometry(
		const Table::ColumnInfo& column_info,
        const Configuration::PhotometricBandMappingConfig::MappingMap& mapping,
	    bool do_marginalize,
	    bool correct_filter,
		bool correct_galactic,
		double dust_map_sed_bpc,
		const std::map<std::string, PhzDataModel::PhotometryGrid>& filter_shift_coef_grid,
	    const std::map<std::string, PhzDataModel::PhotometryGrid>& galactic_correction_coef_grid
	): m_do_marginalize{do_marginalize},
	   m_correct_filter{correct_filter},
	   m_correct_galactic{correct_galactic},
	   m_dust_map_sed_bpc{dust_map_sed_bpc},
	   m_filter_shift_coef_grid{filter_shift_coef_grid},
	   m_galactic_correction_coef_grid{galactic_correction_coef_grid} {

  for (auto& map_filter : mapping) {

	  // Flux
    auto index = column_info.find(map_filter.second.first);
    if (index == nullptr) {
      throw Elements::Exception() << "Input catalog does not contain the column " << map_filter.second.first;
    }

    auto info = Table::ColumnInfo::info_type(map_filter.second.first+"_Corrected", typeid(double));

    m_columnInfo.push_back(info);
    m_indices.push_back(*index);
    // error
     index = column_info.find(map_filter.second.second);
    if (index == nullptr) {
      throw Elements::Exception() << "Input catalog does not contain the column " << map_filter.second.second;
    }

    info = info = Table::ColumnInfo::info_type(map_filter.second.second+"_Corrected", typeid(double));

    m_columnInfo.push_back(info);
    m_indices.push_back(*index);
    // Filter
    m_filters.push_back(map_filter.first);
  }

  logger.info() << "Output corrected photometry = " << m_do_marginalize
		        << " with filter_shift :" << m_correct_filter
		        << " and galactic correction :"<< m_correct_galactic;

}

std::vector<Table::ColumnInfo::info_type> CorrectedPhotometry::getColumnInfoList() const {
  return m_columnInfo;
}


std::vector<double> CorrectedPhotometry::computeCorrectionFactorForModel (
		                               const SourceCatalog::Source& source,
                                       size_t region_index,
		                               const PhzDataModel::PhotometryGrid::const_iterator model) const {
	auto photometry = source.getAttribute<SourceCatalog::Photometry>();
	auto observation_condition_ptr = source.getAttribute<PhzDataModel::ObservationCondition>();
	std::vector<double> full_correction(photometry->size(), 1.0);

	// Filter shift
    if (m_correct_filter) {
		auto filter_map_iter = m_filter_shift_coef_grid.cbegin();
		for (size_t i=0; i<region_index; ++i) {
			++filter_map_iter;
		}

		auto filter_coeff_iter = (*filter_map_iter).second.begin();
		filter_coeff_iter.fixAllAxes(model);
		const std::vector<double>& shifts = observation_condition_ptr->getFilterShifts();
		auto coef_iter = filter_coeff_iter->begin();
		for (size_t index = 0; index < shifts.size(); ++index) {
			double corr = 1.0 + shifts[index]*shifts[index]*(*coef_iter).flux + shifts[index]*(*coef_iter).error;
			full_correction[index] /= corr;
			++coef_iter;
		}
    }

	// Galactic reddening
    if (m_correct_galactic) {
		auto gal_map_iter = m_galactic_correction_coef_grid.cbegin();
		for (size_t i=0; i<region_index; ++i) {
			++gal_map_iter;
		}

		auto galactic_coeff_iter =  (*gal_map_iter).second.begin();
		galactic_coeff_iter.fixAllAxes(model);
		double dust_density = m_dust_map_sed_bpc * observation_condition_ptr->getDustColumnDensity();
		auto gal_coef_iter = galactic_coeff_iter->begin();
		for (size_t index = 0; index < galactic_coeff_iter->size(); ++index) {
			double corr =  std::exp(l10 * -0.4 * (*gal_coef_iter).flux * dust_density);
			full_correction[index] /= corr;
			++gal_coef_iter;
		}
    }

	return full_correction;
}

std::vector<Table::Row::cell_type> CorrectedPhotometry::convertResults(
                  const SourceCatalog::Source& source,
                  const PhzDataModel::SourceResults& sourceResult) const {
  
  auto photometry =	source.getAttribute<SourceCatalog::Photometry>();

  std::vector<Table::Row::cell_type> result {};

  std::vector<double> correction(photometry->size(), 1);
  auto corr_iter = correction.begin();
  for (auto iter = photometry->begin(); iter != photometry->end(); ++iter) {
	  SourceCatalog::FluxErrorPair new_flux_error {*iter};
	  if (m_do_marginalize) {
		  // todo
	  } else {
		  PhzDataModel::PhotometryGrid::const_iterator best_fit_model = sourceResult.get<PhzDataModel::SourceResultType::BEST_MODEL_ITERATOR>();
		  size_t region_index = sourceResult.get<PhzDataModel::SourceResultType::BEST_REGION>();

		  correction = computeCorrectionFactorForModel(source, region_index, best_fit_model);
	  }
	  if (!new_flux_error.missing_photometry_flag){
		  new_flux_error.flux *= *corr_iter;
		  if (new_flux_error.upper_limit_flag){
			  new_flux_error.error = -std::abs(new_flux_error.error);
		  } else {
			  new_flux_error.error *= *corr_iter;
		  }
	  }
	  result.emplace_back(new_flux_error.flux);
	  result.emplace_back(new_flux_error.error);
	  ++corr_iter;
  }
  
  return result;
}

} /* namespace ColumnHandlers */
} /* namespace PhzOutput */
} /* namespace Euclid */
