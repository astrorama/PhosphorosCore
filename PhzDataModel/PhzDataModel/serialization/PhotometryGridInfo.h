/** 
 * @file PhotometryGridInfo.h
 * @date May 11, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H
#define	PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include "GridContainer/serialization/GridContainer.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<typename Archive>
void save(Archive& ar, const Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int) {
  ar << t.axes;
  ar << t.igm_method;
  std::vector<std::string> names_as_strings {};
  for (auto& name : t.filter_names) {
    names_as_strings.push_back(name.qualifiedName());
  }
  ar << names_as_strings;
}

template<typename Archive>
void load(Archive& ar, Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int) {
  ar >> t.axes;
  ar >> t.igm_method;
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

