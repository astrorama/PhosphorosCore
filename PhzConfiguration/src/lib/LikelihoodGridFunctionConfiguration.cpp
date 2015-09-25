/* 
 * @file LikelihoodGridFunctionConfiguration.cpp
 *
 * Created on: September 25, 2015
 *     Author: Nikolaos Apostolakos
 */

#include "ElementsKernel/Exception.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/LikelihoodGridFunctor.h"
#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

#include "PhzConfiguration/LikelihoodGridFunctionConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string ENABLE_MISSING_DATA {"enable-missing-data"};
static const std::string ENABLE_UPPER_LIMIT {"enable-upper-limit"};

po::options_description LikelihoodGridFunctionConfiguration::getProgramOptions() {
  po::options_description options {"Likelihood computation options"};
  options.add_options()
    (ENABLE_MISSING_DATA.c_str(), po::value<std::string>(),
        "Enables or disables the check for missing data. One of OFF, ON(default)")
    (ENABLE_UPPER_LIMIT.c_str(), po::value<std::string>(),
        "Enables or disables the upper limit handling. One of OFF, ON(default)");
  
  return merge(options)
              (PhotometryCatalogConfiguration::getProgramOptions());
}

LikelihoodGridFunctionConfiguration::LikelihoodGridFunctionConfiguration(
                              const std::map<std::string,
                              boost::program_options::variable_value>& options)
        : PhosphorosPathConfiguration(options), CatalogTypeConfiguration(options),
          CatalogConfiguration(options), PhotometryCatalogConfiguration(options) {
  
  std::string missing_data = "ON";
  bool missing_data_flag = true;
  if (options.count(ENABLE_MISSING_DATA) == 1) {
    missing_data = options.at(ENABLE_MISSING_DATA).as<std::string>();
    if (missing_data == "OFF") {
      missing_data_flag = false;
    } else if (missing_data == "ON") {
      missing_data_flag = true;
    } else {
      throw Elements::Exception() << "Unknown " << ENABLE_MISSING_DATA << " option \'" << missing_data << "\'";
    }
  }
  
  std::string upper_limit = "ON";
  bool upper_limit_flag = true;
  if (options.count(ENABLE_UPPER_LIMIT) == 1) {
    upper_limit = options.at(ENABLE_UPPER_LIMIT).as<std::string>();
    if (upper_limit == "OFF") {
      upper_limit_flag = false;
    } else if (upper_limit == "ON") {
      upper_limit_flag = true;
    } else {
      throw Elements::Exception() << "Unknown " << ENABLE_UPPER_LIMIT << " option \'" << upper_limit << "\'";
    }
  }
  
  m_enable_missing_data = false;
  m_enable_upper_limit = false;
  if (missing_data_flag || upper_limit_flag) {
    for (auto& source : getCatalog()) {
      auto photo = source.getAttribute<SourceCatalog::Photometry>();
      if (photo != nullptr) {
        for (auto& filter : *photo) {
          if (missing_data_flag && filter.missing_photometry_flag) {
            m_enable_missing_data = true;
          }
          if (upper_limit_flag && filter.upper_limit_flag) {
            m_enable_upper_limit = true;
          }
        }
      }
    }
  }
}

PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction LikelihoodGridFunctionConfiguration::getLikelihoodGridFunction() {
  PhzLikelihood::LikelihoodLogarithmAlgorithm::ScaleFactorCalc scale_factor {};
  PhzLikelihood::LikelihoodLogarithmAlgorithm::LikelihoodLogarithmCalc likelihood_logarithm {};
  if (m_enable_missing_data) {
    if (m_enable_upper_limit) {
      scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimitMissingData {};
      likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimitMissingData {};
    } else {
      scale_factor = PhzLikelihood::ScaleFactorFunctorMissingData {};
      likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmMissingData {};
    }
  } else {
    if (m_enable_upper_limit) {
      scale_factor = PhzLikelihood::ScaleFactorFunctorUpperLimit {};
      likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmUpperLimit {};
    } else {
      scale_factor = PhzLikelihood::ScaleFactorFunctorSimple {};
      likelihood_logarithm = PhzLikelihood::ChiSquareLikelihoodLogarithmSimple {};
    }
  }
  return PhzLikelihood::LikelihoodGridFunctor {
    PhzLikelihood::LikelihoodLogarithmAlgorithm {
      std::move(scale_factor), std::move(likelihood_logarithm)
    }
  };
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid