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
 * @file CorrectedPhotometryConfig.cpp
 * @author dubathf
 */

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ElementsKernel/Exception.h"
#include "AlexandriaKernel/memory_tools.h"



#include "SourceCatalog/SourceAttributes/TableRowAttributeFromRow.h"
#include "PhzConfiguration/CorrectedPhotometryConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "Configuration/CatalogConfig.h"
#include "Configuration/PhotometricBandMappingConfig.h"
#include "PhzOutput/PhzColumnHandlers/CorrectedPhotometry.h"

#include "PhzConfiguration/FilterVariationCoefficientGridConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridConfig.h"

#include "PhzConfiguration/ObservationConditionColumnConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ENABLE_CORRECTED_PHOTOMETRY {"output-corrected-photometry"};

CorrectedPhotometryConfig::CorrectedPhotometryConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<Euclid::Configuration::PhotometricBandMappingConfig>();
  declareDependency<OutputCatalogConfig>();
  declareDependency<FilterVariationCoefficientGridConfig>();
  declareDependency<CorrectionCoefficientGridConfig>();
  declareDependency<ObservationConditionColumnConfig>();
}

auto CorrectedPhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {ENABLE_CORRECTED_PHOTOMETRY.c_str(), po::value<std::string>()->default_value("NO"),
        "Output the corrected photometry. Possible values are 'NO' - Do not output photometry, "
        "'BESTFIT' - Correct using SED of the best fit model, 'POSTERIOR' marginalize on the full posterior. Default:NO"}
  }}};
}

void CorrectedPhotometryConfig::preInitialize(const UserValues& args){
	std::set<std::string> types {"NO","BESTFIT","POSTERIOR"};
	if (args.count(ENABLE_CORRECTED_PHOTOMETRY) > 0) {
		auto input_type = args.find(ENABLE_CORRECTED_PHOTOMETRY)->second.as<std::string>();

		 if (types.find(input_type) == types.end()) {
		      throw Elements::Exception() << "Unknown "
		          << ENABLE_CORRECTED_PHOTOMETRY << " option \""
		          << input_type << "\"";
		 }
	}
}

void CorrectedPhotometryConfig::initialize(const UserValues& args) {
	auto input_type = args.find(ENABLE_CORRECTED_PHOTOMETRY)->second.as<std::string>();
	if (input_type=="NO") {
		// Nothing to do!
		return;
	}

    const auto&  filter_grid = getDependency<FilterVariationCoefficientGridConfig>().getFilterVariationCoefficientGrid();
    const auto&  milkyway_grid = getDependency<CorrectionCoefficientGridConfig>().getCorrectionCoefficientGrid();

    bool do_marginalize = input_type == "POSTERIOR";
	auto filter_name_mapping = getDependency<Euclid::Configuration::PhotometricBandMappingConfig>().getPhotometricBandMapping();
	double bpc = getDependency<ObservationConditionColumnConfig>().getDustMapSedBpc();
	bool filt_corr =  getDependency<ObservationConditionColumnConfig>().isFilterVariationEnabled();
	bool gal_corr =  getDependency<ObservationConditionColumnConfig>().isGalacticCorrectionEnabled();

	// Add the output handler which will fill the columns
	auto column_info = getDependency<Euclid::Configuration::CatalogConfig>().getColumnInfo();
	getDependency<OutputCatalogConfig>().addColumnHandler(
		  Euclid::make_unique<PhzOutput::ColumnHandlers::CorrectedPhotometry>(
				  *column_info,
				  filter_name_mapping,
				  do_marginalize,
				  filt_corr,
				  gal_corr,
				  bpc,
				  filter_grid,
				  milkyway_grid));
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
