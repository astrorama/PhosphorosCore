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
 * @file tests/src/MultithreadHandler_test.cpp
 * @date 11/23/15
 * @author nikoapos
 */

#include <boost/test/unit_test.hpp>
#include <thread>
#include <algorithm>

#include "PhzOutput/OutputHandler.h"
#include "PhzOutput/MultithreadHandler.h"
#include "PhzUtils/Multithreading.h"

using namespace Euclid;
using namespace Euclid::PhzOutput;

class CheckOrderOutputHandler : public OutputHandler {
  
public:
  
  std::size_t current = 0;
  std::vector<SourceCatalog::Source::id_type> order;
  std::vector<SourceCatalog::Source::id_type> groups;
  std::atomic<size_t>& progress;
  
  CheckOrderOutputHandler(std::atomic<size_t>& arg_progress) : progress(arg_progress) {}
          
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override {
    BOOST_CHECK_EQUAL(current, progress);
    BOOST_CHECK(current < order.size());
    BOOST_CHECK(current < groups.size());
    BOOST_CHECK_EQUAL(source.getId(), order[current]);
    BOOST_CHECK_EQUAL(results.get<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>(), boost::get<int64_t>(source.getId()));
    ++current;
  }
  
};

struct MultithreadHandler_fixture {
  
  std::atomic<size_t> progress {0};
  CheckOrderOutputHandler check_order_handler {progress};
  std::vector<SourceCatalog::Source> source_list {};
  std::vector<PhzDataModel::SourceResults> result_list {};
  std::vector<SourceCatalog::Source::id_type> order {};
  
  MultithreadHandler_fixture() {
    std::vector<SourceCatalog::Source::id_type> groups;
    for (std::int64_t i = 0; i < 10000; ++i) {
      std::int64_t id = i * 10;
      order.emplace_back(id);
      groups.emplace_back(std::to_string(id));
      source_list.emplace_back(SourceCatalog::Source {id, {}});
      result_list.emplace_back();
      result_list.back().set<PhzDataModel::SourceResultType::BEST_MODEL_SCALE_FACTOR>(id);
    }
    check_order_handler.order = order;
    check_order_handler.groups = std::move(groups);
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MultithreadHandler_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sequentialCall, MultithreadHandler_fixture) {

  // Given
  MultithreadHandler handler {check_order_handler, progress, order};
  
  // When
  for (std::size_t i = 0; i < source_list.size(); ++i) {
    handler.handleSourceOutput(source_list.at(i), result_list.at(i));
  }
  
  // Then
  BOOST_CHECK_EQUAL(progress, source_list.size());
  BOOST_CHECK_EQUAL(check_order_handler.current, source_list.size());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(parallelCall, MultithreadHandler_fixture) {

  // Given
  MultithreadHandler handler {check_order_handler, progress, order};
  
  // When
  int thread_no = 100;
  std::vector<std::thread> thread_list {};
  for (int thr_i = 0; thr_i < thread_no; ++thr_i) {
    thread_list.emplace_back([this, &handler, thr_i, thread_no]() {
      for (std::size_t i = thr_i; i < source_list.size(); i=i+thread_no) {
        handler.handleSourceOutput(source_list.at(i), result_list.at(i));
      }
    });
  }
  for (auto& thr : thread_list) {
    thr.join();
  }
  
  // Then
  BOOST_CHECK_EQUAL(progress, source_list.size());
  BOOST_CHECK_EQUAL(check_order_handler.current, source_list.size());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(orderIsRespected, MultithreadHandler_fixture) {

  // Given
  std::random_shuffle(order.begin(), order.end());
  check_order_handler.order = order;
  for (std::size_t i = 0; i < order.size(); ++i) {
    check_order_handler.groups.at(i) = order.at(i);
  }
  MultithreadHandler handler {check_order_handler, progress, order};
  
  // When
  for (std::size_t i = 0; i < source_list.size(); ++i) {
    handler.handleSourceOutput(source_list.at(i), result_list.at(i));
  }
  
  // Then
  BOOST_CHECK_EQUAL(progress, source_list.size());
  BOOST_CHECK_EQUAL(check_order_handler.current, source_list.size());

}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(stopedByTheUser, MultithreadHandler_fixture) {

  // Given
  MultithreadHandler handler {check_order_handler, progress, order};
  
  // When
  for (std::size_t i = 0; i < 100; ++i) {
    handler.handleSourceOutput(source_list.at(i), result_list.at(i));
  }
  
  // Then
  BOOST_CHECK_EQUAL(progress, 100);
  BOOST_CHECK_EQUAL(check_order_handler.current, 100);
  
  // When
  PhzUtils::getStopThreadsFlag() = true;
  
  // Then
  BOOST_CHECK_THROW(handler.handleSourceOutput(source_list.at(101), result_list.at(101)), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


