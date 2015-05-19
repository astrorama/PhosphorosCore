/** 
 * @file CheckPhotometries.h
 * @date May 18, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef CHECKPHOTOMETRIES_H
#define	CHECKPHOTOMETRIES_H

#include <vector>
#include <string>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzConfiguration {

void checkGridPhotometriesMatch(const std::vector<XYDataset::QualifiedName>& grid_names,
                                const std::vector<std::string>& mapping_list);

}
}

#endif	/* CHECKPHOTOMETRIES_H */

