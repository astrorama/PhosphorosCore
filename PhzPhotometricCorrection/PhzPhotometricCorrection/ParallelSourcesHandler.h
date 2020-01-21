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
 * @file PhzPhotometricCorrection/ParallelSourcesHandler.h
 * @date Jan 20, 2020
 * @author Alejandro Alvarez Ayllon
 */

#ifndef PHOTOMETRICCORRECTION_PARALLELSOURCESHANDLER_H
#define PHOTOMETRICCORRECTION_PARALLELSOURCESHANDLER_H

#include "AlexandriaKernel/ThreadPool.h"
#include <cassert>
#include <cmath>

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * Generic wrapper over functors that receive a begin/end iterator and a set of extra parameters
 * @tparam WrappedFunctor
 *  Type of the functor being wrapped
 */
template<typename WrappedFunctor>
class ParallelIteratorHandler {
public:

  /** Copy constructor */
  ParallelIteratorHandler(const ParallelIteratorHandler&) = default;

  /** Move constructor */
  ParallelIteratorHandler(ParallelIteratorHandler&&) = default;

  /** Destructor */
  ~ParallelIteratorHandler() = default;

  /**
   * Constructor for when the constructor of the wrapped functor receives no parameters
   * @param thread_count
   *    Number of threads
   * @param thread_pool
   *    Thread pool. Note that it is received as a parameter to make instances of this class copyable
   */
  ParallelIteratorHandler(unsigned thread_count, ThreadPool& thread_pool) : m_thread_count(thread_count),
                                                                            m_thread_pool(thread_pool), m_wrapped() {}

  /**
   * Constructor
   * @tparam Args
   *    Types of the parameters to forward to the constructor of the wrapped functor
   * @param thread_count
   *    Number of threads
   * @param thread_pool
   *    Thread pool. Note that it is received as a parameter to make instances of this class copyable
   * @param args
   *    Parameters to forward to the constructor of the wrapped functor
   */
  template<typename ...Args>
  ParallelIteratorHandler(unsigned thread_count, ThreadPool& thread_pool, Args&& ...args)
    : m_thread_count(thread_count), m_thread_pool(thread_pool),
      m_wrapped(std::forward<Args>(args)...) {
  }

  /**
   * Wrapping of the operator ()
   * @tparam Iter
   *    Type of the iterator
   * @tparam Args
   *    Type of the additional parameters required by the functor
   * @param begin
   *    Begin iterator
   * @param end
   *    End iterator
   * @param args
   *    Additional parameters required by the functor
   * @return
   *    Same type as the wrapped functor
   */
  template<typename Iter, typename ...Args>
  typename std::result_of<WrappedFunctor(Iter, Iter, Args&& ...)>::type
  operator()(Iter begin, Iter end, Args&& ... args) {
    typedef typename std::result_of<WrappedFunctor(Iter, Iter, Args&& ...)>::type result_t;

    size_t size = end - begin;
    size_t chunk_size = static_cast<size_t>(std::ceil(size / static_cast<float>(m_thread_count)));
    size_t nchunks = static_cast<size_t>(std::ceil(size / static_cast<float>(chunk_size)));

    // Each position of this vector is used by a single thread, so we do not need locking
    std::vector<result_t> intermediate_results(nchunks);

    for (size_t i = 0; i < nchunks; ++i) {
      Iter this_begin = begin + chunk_size * i;
      Iter this_end = this_begin + chunk_size;
      if (this_end > end) {
        this_end = end;
      }

      assert(this_begin < end);
      assert(this_end <= end);

      // This trick allows compiling with older gcc compilers that do not support
      // forwarding variadic arguments inside a lambda
      // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
      auto f = std::bind(m_wrapped, this_begin, this_end, std::cref(args)...);

      m_thread_pool.submit([&intermediate_results, f, i](void) {
        intermediate_results[i] = f();
      });
    }

    // Wait for all to finish
    m_thread_pool.block();

    // Merge results
    result_t results;
    for (auto& r : intermediate_results) {
      results.insert(r.begin(), r.end());
    }

    return results;
  }

private:
  unsigned m_thread_count;
  ThreadPool& m_thread_pool;
  const WrappedFunctor m_wrapped;
};

} /* namespace PhzPhotometricCorrection */
} /* namespace Euclid */

#endif // PHOTOMETRICCORRECTION_PARALLELSOURCESHANDLER_H
