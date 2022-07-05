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
 * @file src/lib/AxisFunctionPriorConfig.cpp
 * @date 01/20/16
 * @author nikoapos
 */

#include "PhzConfiguration/AxisFunctionPriorConfig.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/AxisFunctionPriorProviderConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzLikelihood/AxisFunctionPrior.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string AXIS_FUNCTION_PRIOR{"axis-function-prior"};

static Elements::Logging logger = Elements::Logging::getLogger("AxisFunctionPriorConfig");

AxisFunctionPriorConfig::AxisFunctionPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<AxisFunctionPriorProviderConfig>();
}

auto AxisFunctionPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Axis Prior options",
           {{(AXIS_FUNCTION_PRIOR + "-*").c_str(), po::value<std::vector<std::string>>(),
             "The file containing a dataset with the axis prior"}}}};
}

void AxisFunctionPriorConfig::preInitialize(const UserValues& args) {
  for (auto& axis_name : findWildcardOptions({AXIS_FUNCTION_PRIOR}, args)) {
    if (axis_name != "z" && axis_name != "ebv") {
      throw Elements::Exception() << "Invalid " << AXIS_FUNCTION_PRIOR << "-* "
                                  << "option: * can be one of (z, ebv) but was " << axis_name;
    }
  }
}

namespace {

template <int I>
PhzLikelihood::AxisFunctionPrior<I> createPrior(const std::string& axis_name, const std::string& dataset_name,
                                                XYDataset::XYDatasetProvider& provider) {
  auto dataset = provider.getDataset(axis_name + "/" + dataset_name);
  if (dataset == nullptr) {
    throw Elements::Exception() << "Missing axis prior dataset " << axis_name << "/" << dataset_name;
  }
  auto function = MathUtils::interpolate(*dataset, MathUtils::InterpolationType::LINEAR);
  return PhzLikelihood::AxisFunctionPrior<I>{std::move(function)};
}

}  // namespace

void AxisFunctionPriorConfig::initialize(const UserValues& args) {
  auto provider_ptr = getDependency<AxisFunctionPriorProviderConfig>().getAxisFunctionPriorDatasetProvider();
  for (auto& axis_name : findWildcardOptions({AXIS_FUNCTION_PRIOR}, args)) {
    for (auto& dataset_name : args.at(AXIS_FUNCTION_PRIOR + "-" + axis_name).as<std::vector<std::string>>()) {
      if (axis_name == "z") {
        getDependency<PriorConfig>().addPrior(
            createPrior<PhzDataModel::ModelParameter::Z>(axis_name, dataset_name, *provider_ptr));
      } else if (axis_name == "ebv") {
        getDependency<PriorConfig>().addPrior(
            createPrior<PhzDataModel::ModelParameter::EBV>(axis_name, dataset_name, *provider_ptr));
      }
    }
  }
}

}  // namespace PhzConfiguration
}  // namespace Euclid
