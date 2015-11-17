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

#include "ElementsKernel/Exception.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzConfiguration/MarginalizationConfig.h"

using namespace Euclid::PhzLikelihood;
using namespace Euclid::PhzDataModel;
namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string AXES_COLLAPSE_TYPE {"axes-collapse-type"};

MarginalizationConfig::MarginalizationConfig(long manager_id) : Configuration(manager_id) { }

auto MarginalizationConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Marginalization options", {
    {AXES_COLLAPSE_TYPE.c_str(), po::value<std::string>()->default_value("BAYESIAN"),
        "The method used for collapsing the axes when producing the 1D PDF (one of SUM, MAX, BAYESIAN)"}
  }}};
}

void MarginalizationConfig::preInitialize(const UserValues& args) {
  if (args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "SUM"
      && args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "MAX"
      && args.at(AXES_COLLAPSE_TYPE).as<std::string>() != "BAYESIAN") {
    throw Elements::Exception() << "Unknown " << AXES_COLLAPSE_TYPE << " \""
                    << args.at(AXES_COLLAPSE_TYPE).as<std::string>() << "\"";
  }
}

void MarginalizationConfig::initialize(const UserValues& args) {
  auto collapse_type = args.at(AXES_COLLAPSE_TYPE).as<std::string>();
  if (collapse_type == "SUM") {
    m_marginalization_function = SumMarginalizationFunctor<ModelParameter::Z> { };
  } else if (collapse_type == "MAX") {
    m_marginalization_function = MaxMarginalizationFunctor<ModelParameter::Z> { };
  } else {
    BayesianMarginalizationFunctor_ func {};
    func.setAsNumericalAxis<ModelParameter::EBV>();
    for (auto& corr : m_corrections) {
      func.addCorrection(corr);
    }
    m_marginalization_function = func;
  }
}

void MarginalizationConfig::addMarginalizationCorrection(
            PhzLikelihood::BayesianMarginalizationFunctor_::AxisCorrection corr) {
  if (getCurrentState() >= State::INITIALIZED) {
    throw Elements::Exception() << "addMarginalizationCorrection() call on an "
        << "already initialized MarginalizationConfig";
  }
  m_corrections.emplace_back(std::move(corr));
}

const PhzLikelihood::CatalogHandler::MarginalizationFunction& MarginalizationConfig::getMarginalizationFunc() const {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception()
        << "Call to getMarginalizationFunc() on a not initialized MarginalizationConfig";
  }
  return m_marginalization_function;
}

} // PhzConfiguration namespace
} // Euclid namespace



