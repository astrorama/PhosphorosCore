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
 * @file PhzOutput/MultithreadHandler.h
 * @date 11/23/15
 * @author nikoapos
 */

#ifndef _PHZOUTPUT_MULTITHREADHANDLER_H
#define _PHZOUTPUT_MULTITHREADHANDLER_H

#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <mutex>
#include <atomic>
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

/**
 * @class MultithreadHandler
 * @brief
 *
 */
class MultithreadHandler : public OutputHandler {

public:
  
  MultithreadHandler(PhzOutput::OutputHandler& handler, std::atomic<size_t>& progress,
                     const std::vector<decltype(std::declval<SourceCatalog::Source>().getId())>& order);

  /**
   * @brief Destructor
   */
  virtual ~MultithreadHandler() = default;

  void handleSourceOutput(const SourceCatalog::Source& source, const result_type& results) override;

private:
  
  // This is a helper class which is keeping the results for a single source
  struct ResultType {
    ResultType(const SourceCatalog::Source& source, result_type results)
          : source(source), results(std::move(results)) { }
    ResultType(ResultType&&) = default;
    const SourceCatalog::Source& source;
    result_type results;
  };
  
  PhzOutput::OutputHandler& m_handler;
  std::atomic<size_t>& m_progress;
  std::vector<std::unique_ptr<ResultType>> m_result_list {};
  std::map<decltype(std::declval<SourceCatalog::Source>().getId()), std::size_t> m_index_map {};
  std::mutex m_result_list_mutex {};
  std::mutex m_handler_mutex {};
  std::size_t m_next_to_output_index {0};

}; /* End of MultithreadHandler class */

} /* namespace PhzOutput */
} /* namespace Euclid */


#endif
