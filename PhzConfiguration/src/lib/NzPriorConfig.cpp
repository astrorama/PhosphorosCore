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
 * @file src/lib/NzPriorConfig.cpp
 * @date 2019-03-15
 * @author florian Dubath
 */

#include <set>
#include <map>
#include <vector>
#include "XYDataset/QualifiedName.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/NzPriorConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzNzPrior/NzPrior.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/RegionResults.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string NZ_PRIOR {"Nz-prior"};
static const std::string NZ_PRIOR_BFILTER {"Nz-prior_B_Filter"};
static const std::string NZ_PRIOR_IFILTER {"Nz-prior_I_Filter"};

static const std::string NZ_PRIOR_Z0T1 {"Nz-prior_z0_T1"};
static const std::string NZ_PRIOR_Z0T2 {"Nz-prior_z0_T2"};
static const std::string NZ_PRIOR_Z0T3 {"Nz-prior_z0_T3"};

static const std::string NZ_PRIOR_KMT1 {"Nz-prior_Km_T1"};
static const std::string NZ_PRIOR_KMT2 {"Nz-prior_Km_T2"};
static const std::string NZ_PRIOR_KMT3 {"Nz-prior_Km_T3"};

static const std::string NZ_PRIOR_ALPHAT1 {"Nz-prior_alpha_T1"};
static const std::string NZ_PRIOR_ALPHAT2 {"Nz-prior_alpha_T2"};
static const std::string NZ_PRIOR_ALPHAT3 {"Nz-prior_alpha_T3"};

static const std::string NZ_PRIOR_KT1 {"Nz-prior_K_T1"};
static const std::string NZ_PRIOR_KT2 {"Nz-prior_K_T2"};

static const std::string NZ_PRIOR_FT1 {"Nz-prior_f_T1"};
static const std::string NZ_PRIOR_FT2 {"Nz-prior_f_T2"};

static const std::string NZ_PRIOR_CSTT1 {"Nz-prior_cst_T1"};
static const std::string NZ_PRIOR_CSTT2 {"Nz-prior_cst_T2"};
static const std::string NZ_PRIOR_CSTT3 {"Nz-prior_cst_T3"};


static const std::string NZ_PRIOR_EFFECTIVENESS {"Nz-prior-effectiveness"};


NzPriorConfig::NzPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PriorConfig>();
  declareDependency<SedProviderConfig>();
  declareDependency<FilterProviderConfig>();
  declareDependency<PhotometryGridConfig>();


}

auto NzPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"N(z) Prior options", {
    {NZ_PRIOR.c_str(), po::value<std::string>()->default_value("NO"),
          "If added, turn N(z) Prior on (YES/NO, default: NO)"},
    {NZ_PRIOR_BFILTER.c_str(), po::value<std::string>()->default_value(""),
          "Name of the B filter to be used by the N(z) prior for classifying SED"},
    {NZ_PRIOR_IFILTER.c_str(), po::value<std::string>()->default_value(""),
          "Name of the I fiter to be used by the N(z) prior for classifying SED and compute sources MAG_I. "
          "It must be part of the sources Photometric filters"},

    {NZ_PRIOR_Z0T1.c_str(), po::value<double>()->default_value(0.431),
           "Value for the Z0 param for T1 region (Default=0.431)"},
    {NZ_PRIOR_Z0T2.c_str(), po::value<double>()->default_value(0.390),
          "Value for the Z0 param for T2 region (Default=0.390)"},
    {NZ_PRIOR_Z0T3.c_str(), po::value<double>()->default_value(0.431),
           "Value for the Z0 param for T3 region (Default=0.300)"},

    {NZ_PRIOR_KMT1.c_str(), po::value<double>()->default_value(0.091),
          "Value for the Km param for T1 region (Default=0.091)"},
    {NZ_PRIOR_KMT2.c_str(), po::value<double>()->default_value(0.100),
         "Value for the Km param for T2 region (Default=0.100)"},
    {NZ_PRIOR_KMT3.c_str(), po::value<double>()->default_value(0.150),
          "Value for the Km param for T3 region (Default=0.150)"},

    {NZ_PRIOR_ALPHAT1.c_str(), po::value<double>()->default_value(2.46),
          "Value for the alpha param for T1 region (Default=2.46)"},
    {NZ_PRIOR_ALPHAT2.c_str(), po::value<double>()->default_value(1.81),
         "Value for the alpha param for T2 region (Default=1.81)"},
    {NZ_PRIOR_ALPHAT3.c_str(), po::value<double>()->default_value(2.00),
          "Value for the alpha param for T3 region (Default=2.00)"},

    {NZ_PRIOR_KT1.c_str(), po::value<double>()->default_value(0.4),
          "Value for the K param for T1 region (Default=0.4)"},
    {NZ_PRIOR_KT2.c_str(), po::value<double>()->default_value(0.3),
         "Value for the K param for T2 region (Default=0.3)"},

    {NZ_PRIOR_FT1.c_str(), po::value<double>()->default_value(0.3),
         "Value for the f param for T1 region (Default=0.3)"},
    {NZ_PRIOR_FT2.c_str(), po::value<double>()->default_value(0.35),
        "Value for the f param for T2 region (Default=0.35)"},

    {NZ_PRIOR_CSTT1.c_str(), po::value<double>()->default_value(0.8869),
          "Value for the Cst' param for T1 region (Default=0.8869)"},
    {NZ_PRIOR_CSTT2.c_str(), po::value<double>()->default_value(0.8891),
         "Value for the Cst' param for T2 region (Default=0.8891)"},
    {NZ_PRIOR_CSTT3.c_str(), po::value<double>()->default_value(0.8874),
          "Value for the Cst' param for T3 region (Default=0.8874)"},
    {NZ_PRIOR_EFFECTIVENESS.c_str(), po::value<double>()->default_value(1.),
          "A value in the range [0,1] showing how strongly to apply the N(z) prior"},

  }}};
}

void NzPriorConfig::preInitialize(const UserValues& args) {
  if (args.at(NZ_PRIOR).as<std::string>() != "NO"
      && args.at(NZ_PRIOR).as<std::string>() != "YES") {
    throw Elements::Exception() << "Invalid " + NZ_PRIOR + " value: "
        << args.at(NZ_PRIOR).as<std::string>() << " (allowed values: YES, NO)";
  }

  if (args.at(NZ_PRIOR).as<std::string>() == "YES") {

    if (args.at(NZ_PRIOR_BFILTER).as<std::string>() == "") {
       throw Elements::Exception() << "Missing " << NZ_PRIOR_BFILTER;
    }

    if (args.at(NZ_PRIOR_IFILTER).as<std::string>() == "") {
       throw Elements::Exception() << "Missing " << NZ_PRIOR_IFILTER;
    }

  }

}

void NzPriorConfig::initialize(const UserValues& args) {
  if (args.at(NZ_PRIOR).as<std::string>() == "YES") {

    // Configure the SED classifier
    auto sed_data_provider = getDependency<SedProviderConfig>().getSedDatasetProvider();
    auto filter_data_provider = getDependency<FilterProviderConfig>().getFilterDatasetProvider();
    auto sed_classifier = PhzNzPrior::SedClassifier(filter_data_provider, sed_data_provider);

    // Configure the prior parameters
    double z0_t1 = args.at(NZ_PRIOR_Z0T1).as<double>();
    double z0_t2 = args.at(NZ_PRIOR_Z0T2).as<double>();
    double z0_t3 = args.at(NZ_PRIOR_Z0T3).as<double>();

    double km_t1 = args.at(NZ_PRIOR_KMT1).as<double>();
    double km_t2 = args.at(NZ_PRIOR_KMT2).as<double>();
    double km_t3 = args.at(NZ_PRIOR_KMT3).as<double>();

    double alpha_t1 = args.at(NZ_PRIOR_ALPHAT1).as<double>();
    double alpha_t2 = args.at(NZ_PRIOR_ALPHAT2).as<double>();
    double alpha_t3 = args.at(NZ_PRIOR_ALPHAT3).as<double>();

    double k_t1 = args.at(NZ_PRIOR_KT1).as<double>();
    double k_t2 = args.at(NZ_PRIOR_KT2).as<double>();

    double f_t1 = args.at(NZ_PRIOR_FT1).as<double>();
    double f_t2 = args.at(NZ_PRIOR_FT2).as<double>();

    double cst_t1 = args.at(NZ_PRIOR_CSTT1).as<double>();
    double cst_t2 = args.at(NZ_PRIOR_CSTT2).as<double>();
    double cst_t3 = args.at(NZ_PRIOR_CSTT3).as<double>();

    double effectiveness = args.at(NZ_PRIOR_EFFECTIVENESS).as<double>();

    auto param = PhzNzPrior::NzPriorParam(z0_t1, km_t1, alpha_t1, k_t1, f_t1, cst_t1,
                                       z0_t2, km_t2, alpha_t2, k_t2, f_t2, cst_t2,
                                       z0_t3, km_t3, alpha_t3,             cst_t3);

    // Get the filters
    auto b_filter = XYDataset::QualifiedName{args.at(NZ_PRIOR_BFILTER).as<std::string>()};
    auto i_filter = XYDataset::QualifiedName{args.at(NZ_PRIOR_IFILTER).as<std::string>()};

    // get the sed list
    const std::map<std::string, PhzDataModel::PhotometryGrid>& grids = getDependency<PhotometryGridConfig>().getPhotometryGrid();

    std::vector<XYDataset::QualifiedName> SEDs{};
    for (auto grid_it = grids.begin(); grid_it != grids.end(); ++grid_it) {
      const auto& sed_list = grid_it->second.getAxis<PhzDataModel::ModelParameter::SED>();
      for (auto sed_iter = sed_list.begin(); sed_iter!=  sed_list.end(); ++sed_iter) {
        if (std::find(SEDs.begin(), SEDs.end(), *sed_iter) == SEDs.end()) {
          SEDs.push_back(*sed_iter);
        }
      }
    }

    // Classify the SEDs
    auto sed_groups = sed_classifier(b_filter, i_filter, SEDs);


    // Add the prior
    getDependency<PriorConfig>().addPrior(PhzNzPrior::NzPrior(sed_groups, i_filter, param, effectiveness));

  }
}

} // PhzConfiguration namespace
} // Euclid namespace



