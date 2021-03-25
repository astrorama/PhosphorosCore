/**
 * @file src/lib/ComputeReferenceSampleConfig.cpp
 * @date 08/13/18
 * @author aalvarez
 *
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

#include <Table/AsciiReader.h>
#include <Table/FitsReader.h>
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/BuildReferenceSampleConfig.h"
#include "PhzReferenceSample/ReferenceSample.h"
#include "PhzConfiguration/RedshiftFunctorConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"


namespace po = boost::program_options;
using namespace Euclid::Configuration;
using namespace Euclid::ReferenceSample;


namespace Euclid {
namespace PhzConfiguration {


static const std::string REFSAMPLE_DIR{"reference-sample-dir"};
static const std::string REFSAMPLE_OVERWRITE{"reference-sample-overwrite"};
static const std::string REFSAMPLE_MAXSIZE{"reference-sample-max-file-size"};
static const std::string PHOSPHOROS_CATALOG{"phosphoros-catalog"};
static const std::string PHOSPHOROS_CATALOG_FORMAT{"phosphoros-catalog-format"};


BuildReferenceSampleConfig::BuildReferenceSampleConfig(long manager_id): Configuration(manager_id) {
  declareDependency<SedProviderConfig>();
  declareDependency<ReddeningProviderConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<IgmConfig>();
  declareDependency<RedshiftFunctorConfig>();
  declareDependency<ModelNormalizationConfig>();

}

auto BuildReferenceSampleConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Build NNPZ reference sample", {
    {REFSAMPLE_DIR.c_str(), po::value<std::string>()->required(), "The directory of the reference sample to create"},
    {REFSAMPLE_OVERWRITE.c_str(), po::bool_switch(), "Overwrite the reference sample"},
    {REFSAMPLE_MAXSIZE.c_str(), po::value<size_t>()->default_value(1073741824), "Maximum file size"},
    {PHOSPHOROS_CATALOG.c_str(), po::value<std::string>()->required(), "Filename of the Phosphoros output catalog"},
    {PHOSPHOROS_CATALOG_FORMAT.c_str(), po::value<std::string>()->default_value("FITS"), "Format of the Phosphoros output catalog"}
  }}};
}

void BuildReferenceSampleConfig::preInitialize(const Euclid::Configuration::Configuration::UserValues &args) {
  auto phosphoros_format = args.at(PHOSPHOROS_CATALOG_FORMAT).as<std::string>();
  if (phosphoros_format != "ASCII" && phosphoros_format != "FITS") {
    throw Elements::Exception() << "Invalid value for option " << PHOSPHOROS_CATALOG_FORMAT
                                << ": " << phosphoros_format;
  }
  auto phosphoros_catalog = args.at(PHOSPHOROS_CATALOG).as<std::string>();
  if (!boost::filesystem::exists(phosphoros_catalog)) {
    throw Elements::Exception() << phosphoros_catalog << " does not exist";
  }
}

void BuildReferenceSampleConfig::initialize(const Euclid::Configuration::Configuration::UserValues &args) {
  m_reference_sample_out = args.at(REFSAMPLE_DIR).as<std::string>();
  m_phosphoros_catalog = args.at(PHOSPHOROS_CATALOG).as<std::string>();
  m_catalog_format = args.at(PHOSPHOROS_CATALOG_FORMAT).as<std::string>();
  if (args.count(REFSAMPLE_OVERWRITE))
    m_overwrite = args.at(REFSAMPLE_OVERWRITE).as<bool>();
  m_max_size = args.at(REFSAMPLE_MAXSIZE).as<size_t>();
}

const boost::filesystem::path& BuildReferenceSampleConfig::getReferenceSamplePath() const {
  return m_reference_sample_out;
}

std::unique_ptr<Table::TableReader> BuildReferenceSampleConfig::getPhosphorosCatalogReader() const {
  std::unique_ptr<Table::TableReader> reader;
  if (m_catalog_format == "ASCII") {
   reader.reset(new Table::AsciiReader(m_phosphoros_catalog.native()));
  }
  else {
   reader.reset(new Table::FitsReader(m_phosphoros_catalog.native()));
  }
  return reader;
}

bool BuildReferenceSampleConfig::overwrite() const {
  return m_overwrite;
}

size_t BuildReferenceSampleConfig::getMaxSize() const {
  return m_max_size;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
