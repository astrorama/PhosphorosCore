/**
 * @file PhzDataModel/QualifiedNameGroupManager.h
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H
#define _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H

#include "XYDataset/QualifiedName.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace Euclid {
namespace PhzDataModel {

/**
 * @class QualifiedNameGroupManager
 *
 * @brief
 * Class which organizes XYDataset::QualifiedName objects in groups
 */
class QualifiedNameGroupManager {

public:
  using set_type        = std::unordered_set<XYDataset::QualifiedName>;
  using group_list_type = std::unordered_map<std::string, set_type>;
  using group_type      = std::unordered_map<std::string, set_type>::value_type;

  /**
   * @brief Constructs a new QualifiedNameGroupManager
   * @param groups
   *    The groups to manage
   * @throws Elements::Exception
   *    If any of the given groups overlap
   */
  QualifiedNameGroupManager(group_list_type groups);

  /**
   * @brief Destructor
   */
  virtual ~QualifiedNameGroupManager() = default;

  /// Returns a reference to the qualified name groups managed by the manager
  const group_list_type& getManagedGroups() const;

  /**
   * @brief Returns the group containing the given qualified name
   *
   * @param name
   *    The QualifiedName to search for
   * @return
   *    A pair containing the name of the group and the set of its QualifiedNames
   * @throws Elements::Exception
   *    If no group contains the given qualified name
   */
  const group_type& findGroupContaining(const XYDataset::QualifiedName& name) const;

private:
  group_list_type                                          m_groups;
  std::unordered_map<XYDataset::QualifiedName, group_type> m_reverse_groups;

}; /* End of QualifiedNameGroupManager class */

} /* namespace PhzDataModel */
} /* namespace Euclid */

#endif
