/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */  

/**
 * @file src/lib/MultithreadConfig.cpp
 * @date 11/30/15
 * @author nikoapos
 */

#include "ElementsKernel/Exception.h"
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/MultithreadConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string THREAD_NO {"thread-no"};

MultithreadConfig::MultithreadConfig(long manager_id) : Configuration(manager_id) { }

auto MultithreadConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Multithreading options", {
    {THREAD_NO.c_str(), po::value<int>(), "Number of threads to use"}
  }}};
}

void MultithreadConfig::preInitialize(const UserValues& args) {
  auto param = args.find(THREAD_NO);
  if (param != args.end() && param->second.as<int>() <= 0) {
    throw Elements::Exception() << THREAD_NO << " parameter must be a positive "
                                << "number but was " << param->second.as<int>();
  } 
}

void MultithreadConfig::initialize(const UserValues& args) {
  auto param = args.find(THREAD_NO);
  if (param != args.end()) {
    PhzUtils::getThreadNumber() = param->second.as<int>();
  } 
}

} // PhzConfiguration namespace
} // Euclid namespace



