/** 
 * @file ConfigManager_fixture.h
 * @date Nov 6, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE
#define _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

#include <chrono>
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"

struct ConfigManager_fixture {
  
  long timestamp = Euclid::Configuration::getUniqueManagerId();
  
  Euclid::Configuration::ConfigManager& config_manager = 
                      Euclid::Configuration::ConfigManager::getInstance(timestamp);
  
  template <typename T>
  std::map<std::string, boost::program_options::variable_value> registerConfigAndGetDefaultOptionsMap() {
    config_manager.registerConfiguration<T>();
    auto options = config_manager.closeRegistration();
    
    boost::program_options::variables_map map {};
    auto parsed = boost::program_options::command_line_parser({NULL}, 0)
                        .options(options).run();
    boost::program_options::store(parsed, map);
    
    return map;
  }
 
};

#endif // _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

