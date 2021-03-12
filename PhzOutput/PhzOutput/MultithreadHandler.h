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

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

/**
 * @class MultithreadHandler
 * @brief OutputHandler instance handling results generated from multiple
 * threads
 * @details
 * This class allows for multithreaded calls of the handleSourceOutput() method
 * and it delegates the handling of the results to a wrapped handler. The
 * wrapped handler does not need to be thread safe. This class is handling the
 * synchronization of the calls on it. The order in which the results are
 * handled is guaranteed to respect a given list of source IDs.
 */
class MultithreadHandler : public OutputHandler {
 public:
  /**
   * @brief creates a new MultithreadHandler instance
   * @details
   * The handling of the results is delegated to the given handler. The order
   * of the calls to the handler respects the given order list. The atomic
   * progress parameter is increased each time a source is handled and can be
   * used for monitoring the process.
   * @param handler The OutputHandler to delegate the results
   * @param progress The variable to increase after a result is handled
   * @param order The order in which the results will be handled
   */
  MultithreadHandler(
      PhzOutput::OutputHandler& handler, std::atomic<size_t>& progress,
      const std::vector<typename SourceCatalog::Source::id_type>& order);

  /**
   * @brief Destructor
   */
  virtual ~MultithreadHandler() = default;

  /**
   * @brief Registers a results to be handled
   * @details
   * If this method is called for a source with ID different than the next to
   * be handled, it registers the result for handling and returns (but does not
   * handle result).
   * If it is called with the source for which the result is the next to be
   * handled, it processes the result and any other consequent results up to the
   * first non-registered one.
   * @param source The source for which the results have been calculated
   * @param results The PHZ results
   * @throws Elements::Exception
   *    if the PhzUtils::getStopThreadsFlag() is set
   */
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override;

 private:
  // This is a helper class which is keeping the results for a single source
  struct ResultType {
    ResultType(const SourceCatalog::Source& arg_source,
               PhzDataModel::SourceResults arg_results)
        : source(arg_source), results(std::move(arg_results)) {}
    ResultType(ResultType&&) = default;
    const SourceCatalog::Source& source;
    PhzDataModel::SourceResults results;
  };

  PhzOutput::OutputHandler& m_handler;
  std::atomic<size_t>& m_progress;
  std::vector<std::unique_ptr<ResultType>> m_result_list{};
  std::map<typename SourceCatalog::Source::id_type, std::size_t> m_index_map{};
  std::mutex m_mutex{};
  std::size_t m_next_to_output_index{0};

}; /* End of MultithreadHandler class */

} /* namespace PhzOutput */
} /* namespace Euclid */

#endif /* _PHZOUTPUT_MULTITHREADHANDLER_H */
