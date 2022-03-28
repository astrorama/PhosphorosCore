/**
 * @copyright (C) 2022 Euclid Science Ground Segment
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

#ifndef PHZUTILS_PHZUTILS_PROGRESSREPORTER_H_
#define PHZUTILS_PHZUTILS_PROGRESSREPORTER_H_

#include <ElementsKernel/Logging.h>
#include <chrono>
#include <iomanip>

namespace Euclid {
namespace PhzUtils {

class ProgressReporter {
public:
  explicit ProgressReporter(Elements::Logging& logger) : m_logger(logger) {}

  void operator()(size_t step, size_t total) {
    int  percentage_done = 100. * step / total;
    auto now_time        = std::chrono::steady_clock::now();
    auto time_diff       = now_time - m_last_time;

    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      auto  duration   = std::chrono::steady_clock::now() - m_started;
      float throughput = step / std::chrono::duration<float>(duration).count();

      m_last_progress = percentage_done;
      m_last_done     = step;
      m_last_time     = now_time;
      m_logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ") [" << std::fixed
                      << std::setprecision(2) << throughput << " objects / s]";
    }
  }

private:
  Elements::Logging&                                 m_logger;
  int                                                m_last_progress = -1;
  size_t                                             m_last_done     = 0;
  std::chrono::time_point<std::chrono::steady_clock> m_started       = std::chrono::steady_clock::now(),
                                                     m_last_time     = std::chrono::steady_clock::now();
};

}  // namespace PhzUtils
}  // namespace Euclid

#endif  // PHZUTILS_PHZUTILS_PROGRESSREPORTER_H_
