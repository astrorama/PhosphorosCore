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
 * @file CopyColumnsConfig.cpp
 * @author nikoapos
 */

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ElementsKernel/Exception.h"
#include "AlexandriaKernel/memory_tools.h"

#include "SourceCatalog/SourceAttributes/TableRowAttributeFromRow.h"
#include "PhzConfiguration/CopyColumnsConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "Configuration/CatalogConfig.h"
#include "PhzOutput/PhzColumnHandlers/CopyColumns.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string COPY_COLUMNS {"copy-columns"};

CopyColumnsConfig::CopyColumnsConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<OutputCatalogConfig>();
}

auto CopyColumnsConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Output options", {
    {COPY_COLUMNS.c_str(), po::value<std::string>()->default_value(""),
        "Comma separated names of the input catalog columns to copy to the output"
        "(Use NAME:ALIAS to copy with different name)"}
  }}};
}

void CopyColumnsConfig::initialize(const UserValues& args) {
  
  // First we get the names of the input columns to be copied from the input
  std::vector<std::pair<std::string, std::string>> columns_to_copy {};
  
  // Split the input of the user in tokens
  auto column_list = args.at(COPY_COLUMNS).as<std::string>();
  std::vector<std::string> column_tokens;
  boost::split(column_tokens, column_list, boost::is_any_of(","), boost::token_compress_on);
  
  // Parse each token to get the input and output names
  for (auto& token : column_tokens) {
    if (token == "") {
      continue;
    }
    auto sep = token.find(':');
    if (sep != std::string::npos) {
      columns_to_copy.emplace_back(token.substr(0, sep), token.substr(sep+1, std::string::npos));
    } else {
      columns_to_copy.emplace_back(token, token);
    }
  }
  
  // If we have to columns to copy then we do nothing more
  if (columns_to_copy.empty()) {
    return;
  }
  
  // Add to the catalog the attribute handler for keeping the full raw table row
  getDependency<Euclid::Configuration::CatalogConfig>().addAttributeHandler(
              std::make_shared<SourceCatalog::TableRowAttributeFromRow>());
  
  // Add the output handler which will copy the columns
  auto column_info = getDependency<Euclid::Configuration::CatalogConfig>().getColumnInfo();
  getDependency<OutputCatalogConfig>().addColumnHandler(
          make_unique<PhzOutput::ColumnHandlers::CopyColumns>(*column_info, columns_to_copy));
  
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid