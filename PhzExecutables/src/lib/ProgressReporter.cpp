 /*
 * Copyright (C) 2012-2022 Euclid Science Ground Segment
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
 * @file src/lib/ProgressReporter.cpp
 * @date 2022/05/06
 * @author dubathf
 */


#include <cmath>
#include <iomanip>
#include "PhzExecutables/ProgressReporter.h"

namespace Euclid {
namespace PhzExecutables {

ProgressReporter::ProgressReporter(const Elements::Logging& arg_logger, bool display_perf) :
		m_display_perf{display_perf},
		m_logger{arg_logger} {}

  void ProgressReporter::operator()(size_t step, size_t total) {
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::system_clock;

    int  percentage_done = 100. * step / total;
    auto now_time        = system_clock::now();
    auto time_diff       = duration_cast<duration<float>>(now_time - m_last_time);
    if (percentage_done > m_last_progress || time_diff.count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time     = now_time;

      if (m_display_perf) {
        float obj_per_sec = (step - m_last_done) / time_diff.count();
		if (!std::isfinite(obj_per_sec)) {
		  obj_per_sec = 0.;
		}

		m_last_done = step;
		m_logger.info() << "Parameter space progress: " << percentage_done << " %" << std::fixed << std::setprecision(2)
		    	                       << obj_per_sec << " cells/sec)";
      } else {
    	  m_logger.info() << "Parameter space progress: " << percentage_done << " %";
      }

    }
  }

}
}
