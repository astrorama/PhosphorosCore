

#include "PhzDataModel/LuminosityCorrectionGrid.h"

namespace Euclid {
namespace GridContainer {

std::unique_ptr<PhzDataModel::LuminosityCorrectionCellManager>
      GridCellManagerTraits<PhzDataModel::LuminosityCorrectionCellManager>::factory(size_t size){
  SourceCatalog::LuminosityCorrection default_LuminosityCorrection {{},0.};
  return std::unique_ptr<PhzDataModel::LuminosityCorrectionCellManager> {
    new PhzDataModel::LuminosityCorrectionCellManager(size, default_LuminosityCorrection)
    };
}

size_t GridCellManagerTraits<PhzDataModel::LuminosityCorrectionCellManager>::size(const PhzDataModel::LuminosityCorrectionCellManager& vector) {
    return vector.size();
}

PhzDataModel::LuminosityCorrectionCellManager::iterator GridCellManagerTraits<PhzDataModel::LuminosityCorrectionCellManager>::begin(PhzDataModel::LuminosityCorrectionCellManager& vector) {
    return vector.begin();
}

PhzDataModel::LuminosityCorrectionCellManager::iterator GridCellManagerTraits<PhzDataModel::LuminosityCorrectionCellManager>::end(PhzDataModel::LuminosityCorrectionCellManager& vector) {
  return vector.end();
}


}
}
