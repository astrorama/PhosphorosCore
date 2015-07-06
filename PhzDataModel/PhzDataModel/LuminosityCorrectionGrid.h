/**
 * @file PhzDataModel/LuminosityCorrectionGrid.h

 */

#ifndef PHZDATAMODEL_PHOTOMETRYGRID_H
#define	PHZDATAMODEL_PHOTOMETRYGRID_H

#include <memory>
#include <vector>
#include "SourceCatalog/SourceAttributes/LuminosityCorrection.h"
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::vector<SourceCatalog::LuminosityCorrection> LuminosityCorrectionCellManager;

typedef PhzGrid<LuminosityCorrectionCellManager> LuminosityCorrectionGrid;

} // end of namespace PhzDataModel


namespace GridContainer {

/**
 * @struct Euclid::GridContainer::GridCellManagerTraits<LuminosityCorrectionCellManager>
 * @brief Specialization of the GridCellManagerTraits template.
 *
 * @details
 * We define the GridCellManagerTraits for a vector of LuminosityCorrection to redefine the
 * factory method because the LuminosityCorrection does not have default constructor *
 */
template<>
struct GridCellManagerTraits<PhzDataModel::LuminosityCorrectionCellManager> {
  typedef SourceCatalog::LuminosityCorrection data_type;

  typedef typename PhzDataModel::LuminosityCorrectionCellManager::iterator iterator;

  /**
   * @brief return the size of the LuminosityCorrectionCellManager
   */
  static size_t size(const PhzDataModel::LuminosityCorrectionCellManager& vector);


  /**
   * @brief static iterator on the LuminosityCorrectionCellManager
   */
  static iterator begin(PhzDataModel::LuminosityCorrectionCellManager& vector);

  /**
   * @brief static iterator on the LuminosityCorrectionCellManager
   */
  static iterator end(PhzDataModel::LuminosityCorrectionCellManager& vector);

  /**
   * @brief Factory to build LuminosityCorrectionCellManager which has no default constructor
   *
   * @return
   * A unique_ptr on the new LuminosityCorrectionCellManager.
   *
   */
  static std::unique_ptr<PhzDataModel::LuminosityCorrectionCellManager> factory(size_t size);


  static const bool enable_boost_serialize = true;
}; // end of GridCellManagerTraits

} // end of namespace GridContainer
} // end of namespace Euclid

// Here we include the serialization of the photometry grid. This is done here
// to avoid having the default grid serialization applied to the PhotometryGrid
// (which would happen if the user would forget to include this file)
#include "PhzDataModel/serialization/LuminosityCorrectionGrid.h"

#endif	/* PHZDATAMODEL_PHOTOMETRYGRID_H */

