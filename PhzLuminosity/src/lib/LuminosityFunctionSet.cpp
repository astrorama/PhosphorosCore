/**
 * @file src/lib/LuminosityFunctionSet.cpp
 * @date 19 August 2015
 * @author Florian Dubath
 */

#include <set>
#include <memory>
#include "ElementsKernel/Exception.h"
#include <PhzLuminosity/LuminosityFunctionSet.h>



namespace Euclid {
namespace PhzLuminosity {


LuminosityFunctionSet::LuminosityFunctionSet ( const LuminosityFunctionSet & other){
  for (auto& pair : other.m_luminosity_functions){
    std::unique_ptr<MathUtils::Function> function{pair.second->clone()};
    m_luminosity_functions.push_back(std::make_pair(pair.first,std::move(function)));
  }

}

LuminosityFunctionSet&  LuminosityFunctionSet::operator= ( const LuminosityFunctionSet & other){
  m_luminosity_functions.clear();
  for (auto& pair : other.m_luminosity_functions){
    std::unique_ptr<MathUtils::Function> function{pair.second->clone()};
    m_luminosity_functions.push_back(std::make_pair(pair.first,std::move(function)));
  }
  return *this;
}



LuminosityFunctionSet::LuminosityFunctionSet(
    std::vector<std::pair<LuminosityFunctionValidityDomain,
    std::unique_ptr<MathUtils::Function>>> luminosityFunctions)
: m_luminosity_functions { std::move(luminosityFunctions) } {

  std::set<std::string> processed { };

  for (auto& pair : m_luminosity_functions) {
    std::string group = pair.first.getSedGroupName();
    if (processed.find(group) == processed.end()) {
      processed.emplace(group);

      std::vector<std::pair<double, double>> z_ranges { };
      for (auto& pair_z_search : m_luminosity_functions) {
        if (pair_z_search.first.getSedGroupName() == group) {
          z_ranges.push_back(
              std::make_pair(pair_z_search.first.getMinZ(),
                  pair_z_search.first.getMaxZ()));
        }
      }

      // check for overlap

      for (size_t index_1 = 0; index_1 < z_ranges.size(); ++index_1) {
        for (size_t index_2 = index_1 + 1; index_2 < z_ranges.size(); ++index_2) {
          if (z_ranges[index_1].first < z_ranges[index_2].second) {
            if (z_ranges[index_1].second > z_ranges[index_2].first) {
              throw Elements::Exception()
                  << "In the LuminosityFunctionSet ranges must not overlap";
            }
          }

          if (z_ranges[index_1].second > z_ranges[index_2].first) {
            if (z_ranges[index_1].first < z_ranges[index_2].second) {
              throw Elements::Exception()
                  << "In the LuminosityFunctionSet ranges must not overlap";
            }
          }
        }
      }
    }
  }
}

const std::pair<LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>>&
LuminosityFunctionSet::getLuminosityFunction(const std::string& sedGroup,double z) const{
  for(auto& region :m_luminosity_functions){
      if (region.first.doesApply(sedGroup,z)){
        return region;
      }
   }

  throw Elements::Exception() << "No Luminosity Function is defined for the SED Group '"<< sedGroup
      <<"' at redshift "<<z<<". Please check your configuration.";
}

}
}
