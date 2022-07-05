/**
 * @file ProgramOptionsHelper.cpp
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include <boost/regex.hpp>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include <boost/algorithm/string/predicate.hpp>

namespace po = boost::program_options;

#include <iostream>

namespace Euclid {
namespace PhzConfiguration {

static regex group_regex{"^([^\\s].*):"};
static regex option_regex{"\\s*--([^\\s]+)\\s.*"};

/// The boost program options do not give access to the group names and information
/// We have to do it the hard way, by decoding the help output
std::map<std::string, std::vector<std::string>> getOptionGroups(const po::options_description& options) {
  std::map<std::string, std::vector<std::string>> groups_map{};
  std::stringstream                               help_message{};
  options.print(help_message);
  std::string line;
  std::string group;
  smatch      match_res;
  while (std::getline(help_message, line)) {
    if (regex_match(line, match_res, group_regex)) {
      group             = match_res.str(1);
      groups_map[group] = {};
    } else if (regex_match(line, match_res, option_regex)) {
      groups_map[group].push_back(match_res.str(1));
    }
  }
  return groups_map;
}

/// Merges two options_description objects in a way that keeps the groups
/// information and it avoids duplicates. If any option is duplicated in different
/// groups an Elements::Exception is thrown
po::options_description mergeUniqueOptions(const po::options_description& first,
                                           const po::options_description& second) {

  po::options_description result{};

  auto first_options_groups  = getOptionGroups(first);
  auto second_options_groups = getOptionGroups(second);

  std::set<std::string> groups{};
  for (auto& pair : first_options_groups) {
    groups.insert(pair.first);
  }
  for (auto& pair : second_options_groups) {
    groups.insert(pair.first);
  }
  for (auto& group_name : groups) {
    po::options_description group{group_name};
    std::set<std::string>   options{};
    for (auto& option : first_options_groups[group_name]) {
      options.insert(option);
    }
    for (auto& option : second_options_groups[group_name]) {
      options.insert(option);
    }
    for (auto& option_name : options) {
      auto opt_ptr = std::find_if(first.options().begin(), first.options().end(),
                                  [&](const boost::shared_ptr<po::option_description>& opt) {
                                    return opt->long_name() == option_name;
                                  });
      if (opt_ptr == first.options().end()) {
        opt_ptr = std::find_if(second.options().begin(), second.options().end(),
                               [&](const boost::shared_ptr<po::option_description>& opt) {
                                 return opt->long_name() == option_name;
                               });
      }
      group.add(*opt_ptr);
    }
    result.add(group);
  }

  return result;
}

UniqueOptionsMerger merge(const po::options_description& options) {
  return options;
}

UniqueOptionsMerger UniqueOptionsMerger::operator()(const boost::program_options::options_description& options) const {
  return mergeUniqueOptions(m_options, options);
}

std::set<std::string> findWildcardOptions(const std::vector<std::string>&                  option_name_list,
                                          const std::map<std::string, po::variable_value>& options) {
  std::set<std::string> result;
  for (auto& option_name : option_name_list) {
    for (auto& pair : options) {
      if (boost::starts_with(pair.first, option_name)) {
        auto name = pair.first.substr(option_name.size());
        if (!name.empty()) {
          name = name.substr(1);
        }
        result.insert(name);
      }
    }
  }
  return result;
}

}  // namespace PhzConfiguration
}  // namespace Euclid