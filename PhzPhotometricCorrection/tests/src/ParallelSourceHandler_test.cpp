/**
 * @file ParallelSourcesHandler_test.cpp
 * @date Jan 20, 2020
 * @author Alejandro Alvarez Ayllon
 */

#include "PhzPhotometricCorrection/ParallelSourcesHandler.h"
#include <boost/test/unit_test.hpp>
#include <numeric>

using namespace Euclid::PhzPhotometricCorrection;

/**
 * Dummy functor, just gets each item on an integer vector and multiply by scale
 */
struct Functor {
  typedef typename std::vector<int>::const_iterator Iterator;

  std::map<int, int> operator()(Iterator source, Iterator end, int scale) const {
    std::map<int, int> result;
    for (; source != end; ++source) {
      result.emplace(std::make_pair(*source, *source * scale));
    }
    return result;
  }
};

struct ParallelFixture {
  static constexpr unsigned n_threads = 5;
  Euclid::ThreadPool thread_pool{n_threads};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ParallelSourceHandler_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (ExactChunkSize_test, ParallelFixture) {
  // Send a number of sources that can be divided exactly by the number of threads
  std::vector<int> sources(100);
  std::iota(sources.begin(), sources.end(), 0);

  ParallelIteratorHandler<Functor> handler{n_threads, thread_pool};
  auto results = handler(sources.begin(), sources.end(), 4);

  for (size_t i = 0; i < sources.size(); ++i) {
    BOOST_CHECK_EQUAL(results[i], sources[i] * 4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (RemainderChunk_test, ParallelFixture) {
  // Send a number of sources that can NOT be divided exactly by the number of threads
  std::vector<int> sources(103);
  std::iota(sources.begin(), sources.end(), 0);

  ParallelIteratorHandler<Functor> handler{n_threads, thread_pool};
  auto results = handler(sources.begin(), sources.end(), 4);

  for (size_t i = 0; i < sources.size(); ++i) {
    BOOST_CHECK_EQUAL(results[i], sources[i] * 4);
  }
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE (LessSourcesThanThreads_test, ParallelFixture) {
  // Send less sources than threads
  std::vector<int> sources(2);
  std::iota(sources.begin(), sources.end(), 0);

  ParallelIteratorHandler<Functor> handler{n_threads, thread_pool};
  auto results = handler(sources.begin(), sources.end(), 4);

  for (size_t i = 0; i < sources.size(); ++i) {
    BOOST_CHECK_EQUAL(results[i], sources[i] * 4);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()

//-----------------------------------------------------------------------------
