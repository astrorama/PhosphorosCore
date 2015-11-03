/**
 * @file PhzLikelihood/GroupedAxisCorrection.h
 * @date 11/03/15
 * @author nikoapos
 */

#ifndef _PHZLIKELIHOOD_GROUPEDAXISCORRECTION_H
#define _PHZLIKELIHOOD_GROUPEDAXISCORRECTION_H

#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "PhzDataModel/LikelihoodGrid.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class GroupedAxisCorrection
 * 
 * @brief
 * Class which corrects a likelihood grid for a grouped axis marginalization
 * 
 * @details
 * The grouping is done by the argument o the constructor, which defines the
 * groups. The correction of the likelihood grid is done by diving every cell
 * with the size of its group.
 * 
 * @tparam I
 *    The index of the axis which is corrected for the grouping
 */
template <int I> 
class GroupedAxisCorrection {

public:

  /// Constructs a new instance for the given groups
  GroupedAxisCorrection(PhzDataModel::QualifiedNameGroupManager group_manager);

  /**
   * @brief Destructor
   */
  virtual ~GroupedAxisCorrection() = default;
  
  /**
   * @brief
   * Applies the correction to the given likelihood grid
   * @details
   * The correction applied is that every cell is divided by the size of the
   * group it belongs.
   * @param likelihood_grid
   *    The likelihood grid to correct. This is an input-output parameter
   * @throws Elements::Exception
   *    If the given grid has an axis value that is not belonging to any of the
   *    groups passed at the constructor
   */
  void operator()(PhzDataModel::LikelihoodGrid& likelihood_grid);

private:
  
  PhzDataModel::QualifiedNameGroupManager m_group_manager;

}; /* End of GroupedAxisCorrection class */

} /* namespace PhzLikelihood */
} // namespace Euclid

#include "PhzLikelihood/_impl/GroupedAxisCorrection.icpp"

#endif
