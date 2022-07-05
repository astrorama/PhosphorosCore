/**
 * @file src/lib/QualifiedNameGroupManager.cpp
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "ElementsKernel/Exception.h"
#include <algorithm>

namespace Euclid {
namespace PhzDataModel {

static bool overlapingSets(const std::unordered_set<XYDataset::QualifiedName>& first,
                           const std::unordered_set<XYDataset::QualifiedName>& second) {

  for (auto& it1 : first) {
    if (second.count(it1)) {
      return true;
    }
  }
  return false;
}

QualifiedNameGroupManager::QualifiedNameGroupManager(group_list_type groups) : m_groups{std::move(groups)} {
  // Validate that we do not have overlapping groups
  for (auto it1 = m_groups.begin(); it1 != m_groups.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    for (; it2 != m_groups.end(); ++it2) {
      if (overlapingSets(it1->second, it2->second)) {
        throw Elements::Exception() << "QualifiedName groups " << it1->first << " and " << it2->first << " overlap";
      }
    }

    for (auto& qn : it1->second) {
      m_reverse_groups.emplace(qn, *it1);
    }
  }
}

auto QualifiedNameGroupManager::getManagedGroups() const -> const group_list_type& {
  return m_groups;
}

auto QualifiedNameGroupManager::findGroupContaining(const XYDataset::QualifiedName& name) const -> const group_type& {
  auto it = m_reverse_groups.find(name);
  if (it != m_reverse_groups.end()) {
    return it->second;
  }
  throw Elements::Exception() << "Cannot find '" << name.qualifiedName() << "' in any group";
}

}  // namespace PhzDataModel
}  // namespace Euclid
