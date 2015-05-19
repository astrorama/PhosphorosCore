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

namespace boost {
namespace serialization {

template<typename Archive>
void serialize(Archive& ar, Euclid::PhzDataModel::PhotometryGridInfo& t, const unsigned int) {
  ar & t.axes;
  ar & t.igm_method;
  ar & t.filter_names;
}

} // end of namespace serialization
} // end of namespace boost

#endif	/* PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRIDINFO_H */

