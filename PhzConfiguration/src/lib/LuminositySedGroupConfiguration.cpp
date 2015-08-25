/**
 * @file LuminositySedGroupConfiguration.cpp
 * @date August 20, 2015
 * @author dubathf
 */

#include "PhzConfiguration/ProgramOptionsHelper.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/LuminositySedGroupConfiguration.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_SED_GROUP {"luminosity-sed-group"};

po::options_description LuminositySedGroupConfiguration::getProgramOptions() {
  po::options_description options {"SED group for the Luminosity Function options"};
  options.add_options()
      ((LUMINOSITY_SED_GROUP+"-*").c_str(),po::value<std::string>(),
          "A SED Group defined by its name and the (comma separated) list of SED it contains.");
  return options;
}


PhzLuminosity::SedGroupManager LuminositySedGroupConfiguration::getLuminositySedGroupManager() {
  std::vector<PhzLuminosity::SedGroup> sedGroups { };

  auto group_name_list = findWildcardOptions( { LUMINOSITY_SED_GROUP }, m_options);

  if (group_name_list.size() == 0) {
    throw Elements::Exception()
        << "Missing mandatory configuration: no SED group has been defined for the Luminosity Function.";
  }

  for (auto& group_name : group_name_list) {
    std::string sed_list = m_options[LUMINOSITY_SED_GROUP  +"-"+  group_name].as<std::string>();

    std::vector<std::string> sed_names;

    boost::split(sed_names, sed_list, boost::is_any_of(","),
        boost::token_compress_on);

    std::vector<XYDataset::QualifiedName> seds { };
    for (std::string sed_name : sed_names) {
      seds.emplace_back(XYDataset::QualifiedName(sed_name));
    }

    PhzLuminosity::SedGroup new_group { group_name, std::move(seds) };
    sedGroups.push_back(std::move(new_group));

  }

  return PhzLuminosity::SedGroupManager { sedGroups };
}



}
}
