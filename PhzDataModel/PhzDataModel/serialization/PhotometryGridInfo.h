/** 
 * @file PhotometryGridInfo.h
 * @date May 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H
#define	PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/split_free.hpp>
#include "GridContainer/serialization/GridContainer.h"
#include "GridContainer/serialization/tuple.h"
#include "PhzDataModel/serialization/QualifiedName.h"
#include "PhzDataModel/PhotometryGridInfo.h"

namespace boost {
namespace serialization {

template<typename Archive>
void save(Archive& ar, const Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int) {
  // Store a vector with the region names
  std::vector<std::string> region_names {};
  for (auto& pair : t.region_axes_map) {
    region_names.push_back(pair.first);
  }
  ar<< region_names;
  // Store the axes of each region
  for (auto& name : region_names) {
    ar << t.region_axes_map.at(name);
  }
  // Store the IGM type
  ar << t.igm_method;
  // Store the names of the filters as strings
  std::vector<std::string> filter_names_as_strings {};
  for (auto& name : t.filter_names) {
    filter_names_as_strings.push_back(name.qualifiedName());
  }
  ar << filter_names_as_strings;
}

template<typename Archive>
void load(Archive& ar, Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int) {
  // Read the names of the regions
  std::vector<std::string> region_names {};
  ar >> region_names;
  // Read all the region axes
  for (auto& name : region_names) {
    Euclid::PhzDataModel::ModelAxesTuple axes {{"",{}}, {"",{}}, {"",{}}, {"",{}}};
    ar >> axes;
    t.region_axes_map.emplace(std::make_pair(name, std::move(axes)));
  }
  // Read the IGM type
  ar >> t.igm_method;
  // Read the photometry filters
  std::vector<std::string> names_as_strings;
  ar >> names_as_strings;
  for (auto& name : names_as_strings) {
    t.filter_names.push_back(name);
  }
}

template<typename Archive>
void serialize(Archive& ar, Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int version) {
  split_free(ar, t, version);
}

} // end of namespace serialization
} // end of namespace boost

#endif	/* PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H */

