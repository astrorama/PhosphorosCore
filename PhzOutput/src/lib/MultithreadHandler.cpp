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
 * @file src/lib/MultithreadHandler.cpp
 * @date 11/23/15
 * @author nikoapos
 */

#include "ElementsKernel/Exception.h"
#include "PhzDataModel/Pdf1D.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzOutput/MultithreadHandler.h"
#include "PhzUtils/Multithreading.h"

namespace Euclid {
namespace PhzOutput {

MultithreadHandler::MultithreadHandler(PhzOutput::OutputHandler& handler, std::atomic<size_t>& progress,
                                       const std::vector<decltype(std::declval<SourceCatalog::Source>().getId())>& order)
        : m_handler(handler), m_progress(progress) {
  for (auto id : order) {
    m_index_map[id] = m_result_list.size();
    m_result_list.emplace_back(nullptr);
  }
}

void MultithreadHandler::handleSourceOutput(const SourceCatalog::Source& source, const result_type& results) {
  if (PhzUtils::getStopThreadsFlag()) {
    throw Elements::Exception() << "Stopped by the user";
  }
  
  // Lock the results, add the new result and get all the results that can be
  // flashed to the output handler
  std::vector<std::unique_ptr<ResultType>> to_output {};
  {
    std::lock_guard<std::mutex> results_lock {m_result_list_mutex};
    auto index = m_index_map.find(source.getId());
    if (index == m_index_map.end()) {
      throw Elements::Exception() << "Unexpected source ID while handling output : " << source.getId();
    }
    m_result_list[index->second].reset(new ResultType{source, results});
    
    while (m_next_to_output_index < m_result_list.size() &&
           m_result_list[m_next_to_output_index] != nullptr) {
      to_output.emplace_back(m_result_list[m_next_to_output_index].release());
      ++m_next_to_output_index;
    }
  }
  
  // Lock the output handler and flash the ready outputs
  if (!to_output.empty()) {
    std::lock_guard<std::mutex> handler_lock {m_handler_mutex};
    for (auto& result : to_output) {
      m_handler.handleSourceOutput(result->source, result->results);
      ++m_progress;
    }
  }
}

} // PhzOutput namespace
} // Euclid namespace



