

#ifndef PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H
#define	PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H

#include <boost/serialization/split_free.hpp>
#include "ElementsKernel/Exception.h"
#include "SourceCatalog/SourceAttributes/LuminosityCorrection.h"
#include "GridContainer/serialization/GridContainer.h"
#include "PhzDataModel/LuminosityCorrectionGrid.h"

namespace boost {
namespace serialization {
/**
 * @brief Serialization of the LuminosityCorrectionGrid
 *
 * @details
 * Note the following points: Only non-empty grids can be serialized. All the Correction
 * must have the same reference filter.  the reference filter is stored only once.
 * We store the measurement filter and the correction value for each grid cell.
 *
 */
template<typename Archive>
void save(Archive& ar, const Euclid::PhzDataModel::LuminosityCorrectionGrid& grid, const unsigned int) {
  size_t size = grid.size();
  if (size == 0) {
    throw Elements::Exception() << "Serialization of empty LuminosityCorrectionGrid is not supported";
  }
  // We store the reference filter names only once. We require that all
  // LuminosityCorrection to have the same reference filter.
  std::string filter_name = (*grid.begin()).getReferenceFilter();
  ar << filter_name;

  // We store the measurement filter and the correction value  for each Correction and we check if the
  // filters are matching the common ones
  for (auto& correction : grid) {
      if (correction.getReferenceFilter() != filter_name) {
        throw Elements::Exception() << "Serialization of grids of Luminosity Correction with "
                                    << "different reference filters is not supported";
      }

      ar << correction.getMeasurementFilter();
      ar << correction.getCorrection();
  }
}


/**
 * @brief Deserialization of the LuminosityCorrectionGrid
 *
 * @details
 * One get the filter list then the LuminosityCorrection are reconstructed using a single instance of the filter names.
 * Eventually the Grid cell are populated with the reconstructed LuminosityCorrections.
 *
 */
template<typename Archive>
void load(Archive& ar, Euclid::PhzDataModel::LuminosityCorrectionGrid& grid, const unsigned int) {
  std::string reference_filter_name;
  ar >> reference_filter_name;
  for (auto& cell : grid) {
    std::string measeurement_filter;
    double correction_value;
    ar >> measeurement_filter >> correction_value;
    cell = Euclid::SourceCatalog::LuminosityCorrection {reference_filter_name, measeurement_filter, correction_value};
  }
}

/**
 * @brief split the Boost serialization between the Save and Load functions.
 *
 */
template<typename Archive>
void serialize(Archive& ar, Euclid::PhzDataModel::LuminosityCorrectionGrid& t, const unsigned int version) {
  split_free(ar, t, version);
}

} /* end of namespace serialization */
} /* end of namespace boost */

#endif	/* PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H */

