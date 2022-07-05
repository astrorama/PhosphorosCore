/*
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
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
 *
 */

/**
 * @file PhzExecutables/ProgressReporter.h
 * @date 2022/05/06
 * @author dubathf
 */

#ifndef _PHZEXECUTABLES_PROGRESSREPORTER_H
#define _PHZEXECUTABLES_PROGRESSREPORTER_H

#include "ElementsKernel/Logging.h"
#include <chrono>

namespace Euclid {
namespace PhzExecutables {

/**
 * @class ProgressReporter
 * @brief Format and display the progress of the computation using a Logger, if requested output the speed of the
 * computation
 */
class ProgressReporter {

public:
  /**
   * @brief Constructor
   *
   * @param arg_logger
   * 	Logger used to report the progress
   *
   * @param display_perf
   * 	If true compute the number of element computed / sec and add this information into the log
   */
  ProgressReporter(const Elements::Logging& arg_logger, bool display_perf);

  /**
   * @brief REport the progress
   *
   * @param step
   * 	The current step
   *
   * @param total
   * 	The total number of step
   */
  void operator()(size_t step, size_t total);

private:
  bool                                               m_display_perf;
  int                                                m_last_progress = -1, m_last_done = 0;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();
  Elements::Logging                                  m_logger;
};

}  // namespace PhzExecutables
}  // namespace Euclid

#endif  // _PHZEXECUTABLES_PROGRESSREPORTER_H
