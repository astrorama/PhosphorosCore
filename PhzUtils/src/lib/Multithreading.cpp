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
 * @file src/lib/Multithreading.cpp
 * @date 11/20/15
 * @author nikoapos
 */

#include "PhzUtils/Multithreading.h"
#include <thread>

namespace Euclid {
namespace PhzUtils {

static std::atomic<bool> stop_threads_flag{false};

std::atomic<bool>& getStopThreadsFlag() {
  return stop_threads_flag;
}

static std::atomic<unsigned int> thread_no{std::thread::hardware_concurrency()};

std::atomic<unsigned int>& getThreadNumber() {
  return thread_no;
}

}  // namespace PhzUtils
}  // namespace Euclid
