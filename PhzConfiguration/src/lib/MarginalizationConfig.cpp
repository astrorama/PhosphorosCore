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
 * @file src/lib/MarginalizationConfig.cpp
 * @date 11/17/15
 * @author nikoapos
 */

#include "PhzConfiguration/MarginalizationConfig.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PdfOutputFlagsConfig.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"

using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

namespace {

const std::string AXES_COLLAPSE_TYPE{"axes-collapse-type"};
const std::string LIKELIHOOD_AXES_COLLAPSE_TYPE{"likelihood-axes-collapse-type"};

template <int Parameter>
void addFunctorsToList(PhzDataModel::GridType                                                      grid_type,
                       std::vector<PhzLikelihood::CatalogHandler::MarginalizationFunction>&        func_list,
                       const std::string&                                                          collapse_type,
                       const std::map<int, std::vector<PhzLikelihood::BayesianMarginalizationFunctor<
                                               PhzDataModel::ModelParameter::Z>::AxisCorrection>>& corrections) {
  if (collapse_type == "SUM") {
    func_list.emplace_back(SumMarginalizationFunctor<Parameter>{grid_type});
  } else if (collapse_type == "MAX") {
    func_list.emplace_back(MaxMarginalizationFunctor<Parameter>{grid_type});
  } else {
    BayesianMarginalizationFunctor<Parameter> func{grid_type};
    for (auto& pair : corrections) {
      for (auto& corr : pair.second) {
        func.addCorrection(pair.first, corr);
      }
    }
    func_list.emplace_back(std::move(func));
  }
}

}  // end of anonymous namespace

MarginalizationConfig::MarginalizationConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PdfOutputFlagsConfig>();
}

auto MarginalizationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {
      {"Marginalization options",
       {{AXES_COLLAPSE_TYPE.c_str(), po::value<std::string>()->default_value("BAYESIAN"),
         "The method used for collapsing the axes when producing the 1D PDF (one of SUM, MAX, BAYESIAN)"},
        {LIKELIHOOD_AXES_COLLAPSE_TYPE.c_str(), po::value<std::string>()->default_value("BAYESIAN"),
         "The method used for collapsing the axes when producing the Likelihood 1D PDF (one of SUM, MAX, BAYESIAN)"}}}};
}

void MarginalizationConfig::preInitialize(const UserValues& args) {
  if (args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "SUM" &&
      args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "MAX" &&
      args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "BAYESIAN") {
    throw Elements::Exception() << "Unknown " << AXES_COLLAPSE_TYPE << " \""
                                << args.at(AXES_COLLAPSE_TYPE).as<std::string>() << "\"";
  }

  if (args.at(LIKELIHOOD_AXES_COLLAPSE_TYPE).as<std::string>() != "SUM" &&
      args.at(LIKELIHOOD_AXES_COLLAPSE_TYPE).as<std::string>() != "MAX" &&
      args.at(LIKELIHOOD_AXES_COLLAPSE_TYPE).as<std::string>() != "BAYESIAN") {
    throw Elements::Exception() << "Unknown " << LIKELIHOOD_AXES_COLLAPSE_TYPE << " \""
                                << args.at(LIKELIHOOD_AXES_COLLAPSE_TYPE).as<std::string>() << "\"";
  }
}

void MarginalizationConfig::initialize(const UserValues& args) {
  auto& collapse_type            = args.at(AXES_COLLAPSE_TYPE).as<std::string>();
  auto& likelihood_collapse_type = args.at(LIKELIHOOD_AXES_COLLAPSE_TYPE).as<std::string>();

  auto& flags = getDependency<PdfOutputFlagsConfig>();
  if (flags.pdfSedFlag()) {
    addFunctorsToList<ModelParameter::SED>(PhzDataModel::GridType::POSTERIOR, m_marginalization_func_list,
                                           collapse_type, m_corrections);
  }
  if (flags.pdfRedCurveFlag()) {
    addFunctorsToList<ModelParameter::REDDENING_CURVE>(PhzDataModel::GridType::POSTERIOR, m_marginalization_func_list,
                                                       collapse_type, m_corrections);
  }
  if (flags.pdfEbvFlag()) {
    addFunctorsToList<ModelParameter::EBV>(PhzDataModel::GridType::POSTERIOR, m_marginalization_func_list,
                                           collapse_type, m_corrections);
  }
  // We always compute the redshift 1D PDF even if it is not requested as
  // output, because we perform statistics on it
  addFunctorsToList<ModelParameter::Z>(PhzDataModel::GridType::POSTERIOR, m_marginalization_func_list, collapse_type,
                                       m_corrections);

  if (flags.likelihoodPdfSedFlag()) {
    addFunctorsToList<ModelParameter::SED>(PhzDataModel::GridType::LIKELIHOOD, m_marginalization_func_list,
                                           likelihood_collapse_type, m_corrections);
  }
  if (flags.likelihoodPdfRedCurveFlag()) {
    addFunctorsToList<ModelParameter::REDDENING_CURVE>(PhzDataModel::GridType::LIKELIHOOD, m_marginalization_func_list,
                                                       likelihood_collapse_type, m_corrections);
  }
  if (flags.likelihoodPdfEbvFlag()) {
    addFunctorsToList<ModelParameter::EBV>(PhzDataModel::GridType::LIKELIHOOD, m_marginalization_func_list,
                                           likelihood_collapse_type, m_corrections);
  }
  if (flags.likelihoodPdfZFlag()) {
    addFunctorsToList<ModelParameter::Z>(PhzDataModel::GridType::LIKELIHOOD, m_marginalization_func_list,
                                         likelihood_collapse_type, m_corrections);
  }
}

void MarginalizationConfig::addMarginalizationCorrection(
    int axis, PhzLikelihood::BayesianMarginalizationFunctor<PhzDataModel::ModelParameter::Z>::AxisCorrection corr) {
  if (getCurrentState() >= State::INITIALIZED) {
    throw Elements::Exception() << "addMarginalizationCorrection() call on an "
                                << "already initialized MarginalizationConfig";
  }
  m_corrections[axis].emplace_back(std::move(corr));
}

const std::vector<PhzLikelihood::CatalogHandler::MarginalizationFunction>&
MarginalizationConfig::getMarginalizationFuncList() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "Call to getMarginalizationFunc() on a not initialized MarginalizationConfig";
  }
  return m_marginalization_func_list;
}

}  // namespace PhzConfiguration
}  // namespace Euclid
