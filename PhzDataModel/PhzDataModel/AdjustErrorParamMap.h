/**
 * @file AdjustErrorParamMap.h
 * @date December 1, 2020
 * @author Dubath F
 */

#ifndef PHZDATAMODEL_ADJUSTERRORPARAMMAP_H
#define PHZDATAMODEL_ADJUSTERRORPARAMMAP_H

#include "XYDataset/QualifiedName.h"
#include <map>
#include <tuple>

namespace Euclid {
namespace PhzDataModel {

typedef std::map<XYDataset::QualifiedName, std::tuple<double, double, double>> AdjustErrorParamMap;

/**
 * @brief Write the AdjustErrorParam Map to a stream.
 *
 * @details
 * The serialization is done by writing the map in a table
 * with 4 columns (Filter <string>, Alpha<double>, Beta<double>, Gamma<double>).
 * Then the table is rendered as an ASCII output.
 * The Filter column contains the full (also with groups) filter name.
 * The output can be written in a text file and is human readable.
 *
 * @param out
 * A ostream to which the correction map will be written.
 *
 * @param corrections
 * A AdjustErrorParamMap to be written on the stream.
 */
void writeAdjustErrorParamMap(std::ostream& out, const AdjustErrorParamMap& adjustErrorParam);

/**
 * @brief Read the AdjustErrorParam Map from a stream.
 *
 * @details
 * The stream must contain a ASCII representation of a Table
 * with 4 columns (Filter <string>, Alpha<double>, Beta<double>, Gamma<double>).
 * The Filter column contains the full (also with groups) filter name.
 *
 * Example file:
 * \code
 *  # Column: Filter string
 *  # Column: Alpha double
 *  # Column: Beta double
 *  # Column: Gamma double
 *
 *  MER/Ynir_WFC3f105w    1.00000    0.00000    0.00000
 *   MER/Iext_ACSf775w    2.00000    0.00000    0.00000
 *  MER/Hnir_WFC3f160w    1.00000    1.00000    0.00000
 *   MER/Rext_ACSf606w    1.00000    1.00000    1.00000
 * \endcode
 *
 * @param in
 * A istream encoding the table representing the AdjustErrorParam map.
 *
 * @return
 * A AdjustErrorParamMap read from the stream.
 *
 * @throws ElementsException
 *    If the table contained into the stream do not have columns (Filter,Alpha,Beta,Gamma).
 * @throws ElementsException
 *    If the column Filter is not of type string.
 * @throws ElementsException
 *    If any of the column Alpha,Beta,Gamma is not of type double.

 */
AdjustErrorParamMap readAdjustErrorParamMap(std::istream& in);

}  // end of namespace PhzDataModel
}  // end of namespace Euclid

#endif /* PHZDATAMODEL_ADJUSTERRORPARAMMAP_H */
