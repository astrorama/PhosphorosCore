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
 * @file src/lib/OutputCatalogConfig.cpp
 * @date 07/04/16
 * @author nikoapos
 */

#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/OutputCatalogConfig.h"
#include "PhzConfiguration/PhzOutputDirConfig.h"
#include "PhzOutput/PhzCatalog.h"
#include "PhzOutput/PhzColumnHandlers/Id.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string OUTPUT_CATALOG_FORMAT {"output-catalog-format"};
static const std::string OUTPUT_FLUSH_SIZE {"output-flush-size"};

OutputCatalogConfig::OutputCatalogConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<Euclid::Configuration::CatalogConfig>();
  declareDependency<PhzOutputDirConfig>();
}

auto OutputCatalogConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
return {{"Output options", {
    {OUTPUT_CATALOG_FORMAT.c_str(), po::value<std::string>()->default_value("ASCII"),
        "The format of the PHZ catalog file (one of ASCII (default), FITS)"},
    {OUTPUT_FLUSH_SIZE.c_str(), po::value<uint>()->default_value(500),
        "The chunk size with which the results are flushed to the files"}
  }}};
}

void OutputCatalogConfig::preInitialize(const UserValues& args) {

  if (args.at(OUTPUT_CATALOG_FORMAT).as<std::string>() != "ASCII" &&
      args.at(OUTPUT_CATALOG_FORMAT).as<std::string>() != "FITS") {
    throw Elements::Exception() << "Invalid value for option " << OUTPUT_CATALOG_FORMAT
        << ": " << args.at(OUTPUT_CATALOG_FORMAT).as<std::string>();
  }
  
  if (args.at(OUTPUT_FLUSH_SIZE).as<uint>() <= 0) {
    throw Elements::Exception() << "Option " << OUTPUT_FLUSH_SIZE << " must be "
        << "positive, but was: " << args.at(OUTPUT_FLUSH_SIZE).as<int>();
  }

}

void OutputCatalogConfig::initialize(const UserValues& args) {
  auto output_dir = getDependency<PhzOutputDirConfig>().getPhzOutputDir();
  auto catalog_config =  getDependency<Euclid::Configuration::CatalogConfig>();

  // The ID is always a part of the catalog
  auto column_info = catalog_config.getColumnInfo();
  auto id_column = catalog_config.getIdColumn();
  auto id_index = column_info->find(id_column);
  if (!id_index) {
    // This really should have happened sooner
    throw Elements::Exception() << "Could not find the ID column on the input catalog";
  }
  auto id_info = column_info->getDescription(*id_index);
  m_column_handler_list.emplace_back(new PhzOutput::ColumnHandlers::Id {id_info.type});

  if (args.at(OUTPUT_CATALOG_FORMAT).as<std::string>() == "ASCII") {
    m_format = PhzOutput::PhzCatalog::Format::ASCII;
    m_out_catalog_file = output_dir / "phz_cat.txt";
  } else {
    m_format = PhzOutput::PhzCatalog::Format::FITS;
    m_out_catalog_file = output_dir / "phz_cat.fits";
  }
  
  m_flush_size = args.at(OUTPUT_FLUSH_SIZE).as<uint>();
}

std::unique_ptr<PhzOutput::OutputHandler> OutputCatalogConfig::getOutputHandler() const {
  if (getCurrentState() < Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to getOutputHandler() on a not initialized instance.";
  }

  return std::unique_ptr<PhzOutput::OutputHandler> {
    new PhzOutput::PhzCatalog {m_out_catalog_file, m_format, m_column_handler_list, m_comments, m_flush_size}
  };
}

void OutputCatalogConfig::addColumnHandler(std::unique_ptr<PhzOutput::ColumnHandler> handler) {
  if (getCurrentState() >= Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to addColumnHandler() on an already initialized instance.";
  }
  m_column_handler_list.emplace_back(std::move(handler));
}

void OutputCatalogConfig::addComment(std::string comment) {
  if (getCurrentState() >= Configuration::Configuration::State::FINAL) {
    throw Elements::Exception()
        << "Call to addComment() on an already initialized instance.";
  }
  m_comments.emplace_back(std::move(comment));
}

uint OutputCatalogConfig::getFlushSize() const {
  return m_flush_size;
}

} // PhzConfiguration namespace
} // Euclid namespace



