/**
 * @file src/lib/QualifiedNameGroupManager.cpp
 * @date 11/02/15
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzDataModel {

// This method takes advantage that the two sets are ordered to check if they
// overlap. It is faster than the std::set_intersection because it returns right
// after the first common element and because it does not require memory
// allocations for the result.
static bool overlapingSets(const std::set<XYDataset::QualifiedName>& first,
                           const std::set<XYDataset::QualifiedName>& second) {
  auto it1 = first.begin();
  auto it2 = second.begin();
  while (it1 != first.end() && it2 != second.end()) {
    if (*it1 < *it2) {
      ++it1;
    } else if (*it2 < * it1) {
      ++ it2;
    } else {
      return true;
    }
  }
  return false;
}

QualifiedNameGroupManager::QualifiedNameGroupManager(group_list_type groups)
        : m_groups{std::move(groups)} {
  // Validate that we do not have overlapping groups
  for (auto it1 = m_groups.begin(); it1 != m_groups.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    for (; it2 != m_groups.end(); ++it2) {
      if (overlapingSets(it1->second, it2->second)) {
        throw Elements::Exception() << "QualifiedName groups " << it1->first
            << " and " << it2->first << " overlap";
      }
    }
  }
}

auto QualifiedNameGroupManager::getManagedGroups() const -> const group_list_type& {
  return m_groups;
}

auto QualifiedNameGroupManager::findGroupContaining(const XYDataset::QualifiedName& name) const -> const group_type& {
  for (auto& pair : m_groups) {
    if (pair.second.find(name) != pair.second.end()) {
      return pair;
    }
  }
  throw Elements::Exception() << "Cannot find '" << name.qualifiedName()
                              << "' in any group";
}

} // PhzDataModel namespace
} // Euclid namespace



