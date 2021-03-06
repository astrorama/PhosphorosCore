/**
 * @file CheckPhotometries.h
 * @date May 18, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef CHECKPHOTOMETRIES_H
#define CHECKPHOTOMETRIES_H

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "XYDataset/QualifiedName.h"
#include <string>
#include <vector>

namespace Euclid {
namespace PhzConfiguration {

void checkGridPhotometriesMatch(const std::vector<XYDataset::QualifiedName>& grid_names,
                                const std::vector<std::string>&              mapping_list);

void checkHaveAllCorrections(const PhzDataModel::PhotometricCorrectionMap& phot_corr_map,
                             const std::vector<std::string>&               mapping_list);

}  // namespace PhzConfiguration
}  // namespace Euclid

#endif /* CHECKPHOTOMETRIES_H */
