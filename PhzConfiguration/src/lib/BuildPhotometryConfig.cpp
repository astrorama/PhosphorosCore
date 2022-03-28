/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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

#include "PhzConfiguration/BuildPhotometryConfig.h"
#include <boost/filesystem/operations.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static const std::string REFSAMPLE_DIR{"reference-sample-dir"};
static const std::string OUT_FILE{"out-file"};
static const std::string OVERWRITE{"overwrite"};
static const std::string INPUT_SIZE{"input-size"};
static const std::string CHUNK_SIZE{"chunk-size"};

BuildPhotometryConfig::BuildPhotometryConfig(long manager_id) : Configuration::Configuration(manager_id) {}

auto BuildPhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Build Photometry Config Arguments",
           {{REFSAMPLE_DIR.c_str(), po::value<std::string>()->required(), "The reference sample directory"},
            {OUT_FILE.c_str(), po::value<std::string>()->required(), "The output FITS file where to write the photometry"},
            {OVERWRITE.c_str(), po::bool_switch()->default_value(false), "Overwrite the output file"},
            {INPUT_SIZE.c_str(), po::value<int>(), "Limit the computation to this many sources"},
            {CHUNK_SIZE.c_str(), po::value<int>()->default_value(100), "Chunk size"}}}};
}

void BuildPhotometryConfig::initialize(const UserValues& args) {
  if (args.count(INPUT_SIZE)) {
    m_input_size = args.at(INPUT_SIZE).as<int>();
  }
  if (args.count(OVERWRITE)) {
    m_overwrite = args.at(OVERWRITE).as<bool>();
  }
  m_output_catalog = args.at(OUT_FILE).as<std::string>();
  if (fs::exists(m_output_catalog) && !m_overwrite) {
    throw Elements::Exception() << m_output_catalog << " exists and overwrite is disabled";
  }

  auto reference_sample_path = args.at(REFSAMPLE_DIR).as<std::string>();
  m_reference_sample         = std::make_unique<ReferenceSample::ReferenceSample>(reference_sample_path);
  if (m_input_size == 0) {
    m_input_size = m_reference_sample->size();
  }
  m_chunk_size = args.at(CHUNK_SIZE).as<int>();
}

size_t BuildPhotometryConfig::getInputSize() const {
  return m_input_size;
}

size_t BuildPhotometryConfig::getChunkSize() const {
  return m_chunk_size;
}

const boost::filesystem::path& BuildPhotometryConfig::getOutputPath() const {
  return m_output_catalog;
}

const ReferenceSample::ReferenceSample& BuildPhotometryConfig::getReferenceSample() const {
  return *m_reference_sample;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
