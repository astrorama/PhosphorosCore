/*
 * SedGroup.h
 *
 *  Created on: Aug 18, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUP_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUP_H_

#include <vector>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::SedGroup
 *
 * @brief Class used to store the SED Group.
 */
class SedGroup{
public:
  /**
   * @brief constructor
   *
   * @param groupName
   * The name of the group to be created
   *
   * @param seds
   * The list of SED which are part of the group.
   */
  SedGroup(const std::string& groupName, std::vector<XYDataset::QualifiedName> seds);

  virtual ~SedGroup() = default;

  /**
   * @short Getter on the Group name
   */
  std::string getName() const;

  /**
   * @short Compute the number of SED into the Group
   */
  int getSedsNumber() const;

  /**
   * @brief Check if a given SED is part of the group
   *
   * @param sed
   * The SED Qualified Name to be check to belong to the group
   */
  bool isInGroup(const XYDataset::QualifiedName& sed) const;

  /**
   * @brief Getter on the list of SED Name. To be used for persistance purpose.
   */
  std::vector<std::string> getSedNameList() const;
private:
  std::string m_group_name;
  std::vector<XYDataset::QualifiedName> m_seds;
};

}
}

#endif /* PHZLUMINOSITY_PHZLUMINOSITY_SEDGROUP_H_ */
