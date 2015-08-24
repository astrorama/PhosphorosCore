/*
 * SedGroupManager.h
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUPMANAGER_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUPMANAGER_H_

#include <vector>
#include "PhzLuminosity/SedGroup.h"

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::SedGroupManager
 *
 * @brief Class used to store the SED Groups.
 */
class SedGroupManager{
public:
  /**
   * @brief Constructor
   *
   * @param sedGroups
   * List of SED Group to be handled by the manager
   */
  SedGroupManager(std::vector<SedGroup> sedGroups);

  SedGroupManager(const SedGroupManager & other)=default;

  SedGroupManager&  operator= ( const SedGroupManager & other)=default;

  virtual ~SedGroupManager() = default;

  /**
   * @brief Return the SED Group a given SED belongs to.
   * Throw an Exception if the SED do not belong to any group.
   *
   * @param sed
   * The SED one is looking for the group it belong to
   */
  std::string getGroupName(XYDataset::QualifiedName sed) const;

  /**
   * @brief Return the list of SED name belonging to a given Group.
   * Throw an Exception if no group with the provided name is found.
   *
   * @param groupName
   * The name of the group one want the SED list.
   */
  std::vector<std::string> getGroupSeds(std::string groupName) const;

private:
  std::vector<SedGroup> m_sed_groups;
};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUPMANAGER_H_ */
