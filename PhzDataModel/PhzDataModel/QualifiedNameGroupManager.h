/**
 * @file PhzDataModel/QualifiedNameGroupManager.h
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H
#define _PHZDATAMODEL_QUALIFIEDNAMEGROUPMANAGER_H

#include <set>
#include <map>
#include <string>
#include <utility>
#include "XYDataset/QualifiedName.h"

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
  
  using group_list_type = std::map<std::string, std::set<XYDataset::QualifiedName>>;
  using group_type = std::map<std::string, std::set<XYDataset::QualifiedName>>::value_type;

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
  
  std::map<std::string, std::set<XYDataset::QualifiedName>> m_groups;

}; /* End of QualifiedNameGroupManager class */

} /* namespace PhzDataModel */
} /* namespace Euclid */


#endif
