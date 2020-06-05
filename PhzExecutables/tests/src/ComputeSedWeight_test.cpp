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
 * @file tests/src/ComputeSedWeight_test.cpp
 * @date 2020/06/03
 * @author dubathf
 */

#include <boost/test/unit_test.hpp>
#include <vector>
#include "PhzExecutables/ComputeSedWeight.h"
#include "XYDataset/XYDataset.h"

using namespace Euclid;
using namespace Euclid::PhzExecutables;



class MockDatasetProvider: public Euclid::XYDataset::XYDatasetProvider{
  public:
  MockDatasetProvider(MockDatasetProvider&&) = default;

  MockDatasetProvider& operator=(MockDatasetProvider&&) = default;

  MockDatasetProvider() {

    }

  MockDatasetProvider(std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset>&& storage):m_storage(std::move(storage)){}

    std::unique_ptr<Euclid::XYDataset::XYDataset> getDataset(const Euclid::XYDataset::QualifiedName& qualified_name) override {
      std::vector<std::pair<double, double>> copied_values {};
      try {
        for (auto& sed_pair : m_storage.at(qualified_name)) {
          copied_values.push_back( std::make_pair(sed_pair.first,sed_pair.second));
        }
      } catch (std::out_of_range&){
        return nullptr;
      }
      return std::unique_ptr<Euclid::XYDataset::XYDataset>{new Euclid::XYDataset::XYDataset(std::move(copied_values))};
    }

    std::vector<Euclid::XYDataset::QualifiedName> listContents(const std::string&) override {
      std::vector<Euclid::XYDataset::QualifiedName> content{};
      for (auto& pair : m_storage) {
        content.push_back(pair.first);
      }

      return content;
    }

    std::string getParameter(const Euclid::XYDataset::QualifiedName& qualified_name, const std::string& key_word) override {
       return key_word;
    }

  private:
    std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> m_storage{};
  };









//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ComputeSedWeight_test)

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(distance_test) {
  ComputeSedWeight computer = ComputeSedWeight();

  // WHEN
  std::vector<double> color_1{1.0, 1.2, 1.0};
  std::vector<double> color_2{1.1, 1.0, 1.3};

  // Distance computed is the max of the abs of the diff
  // THEN
  BOOST_CHECK_CLOSE(0.3, computer.distance(color_1, color_2), 0.00001);
  BOOST_CHECK_CLOSE(0.3, computer.distance(color_2, color_1), 0.00001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(distance_same_test) {
  ComputeSedWeight computer = ComputeSedWeight();

  // WHEN
  std::vector<double> color_1{1.0, 1.2, 1.0};

  // Distance computed is the max of the abs of the diff
  // THEN
  BOOST_CHECK_CLOSE(0.0, computer.distance(color_1, color_1), 0.00001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(seds_distance_test) {
  ComputeSedWeight computer = ComputeSedWeight();

  // WHEN
  std::vector<double> color_1 = std::vector<double>{1.0, 1.2, 1.1};
  std::vector<double> color_2 = std::vector<double>{1.1, 1.2, 1.3};
  std::vector<double> color_3 = std::vector<double>{1.4, 1.2, 1.0};

  std::vector<std::vector<double>> colors{color_1, color_2, color_3};

  // THEN
  std::vector<std::vector<double>> distances = computer.computeSedDistance(colors);

  BOOST_CHECK_EQUAL(3, distances.size());
  BOOST_CHECK_EQUAL(3, distances[0].size());

  // diagonal
  BOOST_CHECK_CLOSE(1000.0, distances[0][0], 0.00001);
  BOOST_CHECK_CLOSE(1000.0, distances[1][1], 0.00001);
  BOOST_CHECK_CLOSE(1000.0, distances[2][2], 0.00001);

  // upper off diagonal
  BOOST_CHECK_CLOSE(0.2, distances[0][1], 0.00001);
  BOOST_CHECK_CLOSE(0.4, distances[0][2], 0.00001);
  BOOST_CHECK_CLOSE(0.3, distances[1][2], 0.00001);

  // symetry
  BOOST_CHECK_CLOSE(distances[1][0], distances[0][1], 0.00001);
  BOOST_CHECK_CLOSE(distances[2][0], distances[0][2], 0.00001);
  BOOST_CHECK_CLOSE(distances[2][1], distances[1][2], 0.00001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(group_distance_test) {
  ComputeSedWeight computer = ComputeSedWeight();
  // WHEN
  /*
   / 1000  0.2   0.4 \
  |   0.2 1000   0.3  |
   \  0.4  0.3  1000 /
  */
  std::vector<std::vector<double>> sed_distances{{1000, 0.2,  0.4}, {0.2, 1000, 0.3}, {0.4, 0.3, 1000}};

  // Group Distance is the smallest distance between the elements of different groups
  // THEN
  BOOST_CHECK_CLOSE(0.2, computer.groupDistance({0}, {1}, sed_distances), 0.00001);
  BOOST_CHECK_CLOSE(0.4, computer.groupDistance({0}, {2}, sed_distances), 0.00001);
  BOOST_CHECK_CLOSE(0.2, computer.groupDistance({0}, {1, 2}, sed_distances), 0.00001);

  BOOST_CHECK_CLOSE(0.3, computer.groupDistance({1}, {2}, sed_distances), 0.00001);
  BOOST_CHECK_CLOSE(0.3, computer.groupDistance({0, 1}, {2}, sed_distances), 0.00001);
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(maxGap_test) {
  ComputeSedWeight computer = ComputeSedWeight();
  // WHEN
  /*
   / 1000  0.2   0.4 \
  |   0.2 1000   0.3  |
   \  0.4  0.3  1000 /
  */
  std::vector<std::vector<double>> sed_distances{{1000, 0.2,  0.4}, {0.2, 1000, 0.3}, {0.4, 0.3, 1000}};

  // THEN
  BOOST_CHECK_CLOSE(0.3, computer.maxGap(sed_distances), 0.00001);

  // 5 seds on a row => easy distance computation
  //  0    1    2            3     4     index
  //-----------------------------------
  //  0   0.2  0.4          0.7   0.75  position
  // should group 0,1,2 and 3,4 max gap between 2 and 3 = 0.4

   std::vector<std::vector<double>> sed_distances_2 {{1000, 0.2,  0.3,  0.7,  0.75},
                                                     {0.2,  1000, 0.1,  0.5,  0.55},
                                                     {0.3,  0.1,  1000, 0.4,  0.45},
                                                     {0.7,  0.5,  0.4,  1000, 0.05},
                                                     {0.75, 0.55, 0.45, 0.05, 1000}};

   // THEN
   BOOST_CHECK_CLOSE(0.4, computer.maxGap(sed_distances_2), 0.00001);
}


//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(getWeights_simple_test) {
  ComputeSedWeight computer = ComputeSedWeight(5000000);

   std::vector<std::vector<double>> seds_colors {{1, 1},
                                                 {2, 1},
                                                 {4, 1}};

   // THEN
   std::vector<double> weights = computer.getWeights(seds_colors, 0.5);
   // No overlap => same weight
   for (size_t i = 0 ; i < weights.size(); ++i) {
     BOOST_CHECK_CLOSE(0.33333, weights[i], 1);
   }

   weights = computer.getWeights(seds_colors, 1);
   // overlap from 0 & 1
   BOOST_CHECK_CLOSE(1.0/5.0 + 1.0/10.0, weights[0], 1);
   BOOST_CHECK_CLOSE(1.0/5.0 + 1.0/10.0, weights[1], 1);
   BOOST_CHECK_CLOSE(2.0/5.0, weights[2], 1);

}

BOOST_AUTO_TEST_CASE(getWeights_test) {
  ComputeSedWeight computer = ComputeSedWeight(5000000);

  // on color 1 5 sed on a row on color 2 same color
  //  0    1    2            3     4     index
  //-----------------------------------
  //  0   0.2  0.4          0.7   0.75  position


   std::vector<std::vector<double>> seds_colors {{0,    1},
                                                {0.2,  1},
                                                {0.3,  1},
                                                {0.7,  1},
                                                {0.75, 1}};

   // THEN
   std::vector<double> weights = computer.getWeights(seds_colors, 0.025);
   // No overlap => same weight
   for (size_t i = 0 ; i < weights.size(); ++i) {
     BOOST_CHECK_CLOSE(0.2, weights[i], 1);
   }

   weights = computer.getWeights(seds_colors, 0.05);
   // last 2 overlaps sharing half of the interval
   BOOST_CHECK_CLOSE(2.0/9.0, weights[0], 1);
   BOOST_CHECK_CLOSE(2.0/9.0, weights[1], 1);
   BOOST_CHECK_CLOSE(2.0/9.0, weights[2], 1);
   BOOST_CHECK_CLOSE(1.0/6.0, weights[3], 1);
   BOOST_CHECK_CLOSE(1.0/6.0, weights[4], 1);

}



//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(order_filter_test) {
  ComputeSedWeight computer = ComputeSedWeight();
  XYDataset::QualifiedName f1{"filter_1"};
  XYDataset::QualifiedName f2{"filter_2"};
  XYDataset::QualifiedName f3{"filter_3"};

  XYDataset::XYDataset xyf1 = XYDataset::XYDataset::factory({0.0, 9.0, 9.1, 10.9, 11.0, 40.0},
                                                            {0.0, 0.0, 1.0,  1.0,  0.0,  0.0});


  XYDataset::XYDataset xyf2 = XYDataset::XYDataset::factory({0.0,29.0,29.1,30.9,31.0,40.0},
                                                            {0.0,0.0,1.0, 1.0, 0.0, 0.0});


  XYDataset::XYDataset xyf3 = XYDataset::XYDataset::factory({0.0,19.0,19.1,20.9,21.0,40.0},
                                                            {0.0,0.0,1.0, 1.0, 0.0, 0.0});


  std::vector<XYDataset::QualifiedName> filter_list{f1, f2, f3};

  std::map<Euclid::XYDataset::QualifiedName, XYDataset::XYDataset> storage;

  storage.insert(std::make_pair(f1, xyf1));
  storage.insert(std::make_pair(f2, xyf2));
  storage.insert(std::make_pair(f3, xyf3));



  auto filter_provider = std::shared_ptr<XYDataset::XYDatasetProvider>{new MockDatasetProvider{std::move(storage)}};


  std::vector<std::pair<XYDataset::QualifiedName, double>> ordered = computer.orderFilters(filter_list, filter_provider);

  BOOST_CHECK_EQUAL("filter_1", ordered[0].first.qualifiedName());
  BOOST_CHECK_EQUAL("filter_3", ordered[1].first.qualifiedName());
  BOOST_CHECK_EQUAL("filter_2", ordered[2].first.qualifiedName());

  BOOST_CHECK_CLOSE(10, ordered[0].second, 0.5);
  BOOST_CHECK_CLOSE(20, ordered[1].second, 0.5);
  BOOST_CHECK_CLOSE(30, ordered[2].second, 0.5);

}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(computeSedColors_test) {
  ComputeSedWeight computer = ComputeSedWeight();

  XYDataset::QualifiedName f1{"filter_1"};
  XYDataset::QualifiedName f2{"filter_2"};
  XYDataset::QualifiedName f3{"filter_3"};

  XYDataset::XYDataset xyf1 = XYDataset::XYDataset::factory({0.0, 9.0, 9.1, 10.9, 11.0, 40.0}, {0.0, 0.0, 1.0,  1.0,  0.0,  0.0});
  XYDataset::XYDataset xyf2 = XYDataset::XYDataset::factory({0.0, 19.0, 19.1, 20.9, 21.0, 40.0}, {0.0, 0.0, 1.0, 1.0, 0.0, 0.0});
  XYDataset::XYDataset xyf3 = XYDataset::XYDataset::factory({0.0, 29.0, 29.1, 30.9, 31.0, 40.0}, {0.0, 0.0, 1.0, 1.0, 0.0, 0.0});



  std::map<Euclid::XYDataset::QualifiedName, XYDataset::XYDataset> storage;
  storage.insert(std::make_pair(f1, xyf1));
  storage.insert(std::make_pair(f2, xyf2));
  storage.insert(std::make_pair(f3, xyf3));
  auto filter_provider = std::shared_ptr<XYDataset::XYDatasetProvider>{new MockDatasetProvider{std::move(storage)}};
  std::vector<std::pair<XYDataset::QualifiedName, double>> ordered;
  ordered.push_back(std::make_pair(f1, 10.0));
  ordered.push_back(std::make_pair(f2, 20.0));
  ordered.push_back(std::make_pair(f3, 30.0));

  XYDataset::QualifiedName s1{"sed_1"};
  XYDataset::QualifiedName s2{"sed_2"};
  XYDataset::XYDataset xys1 = XYDataset::XYDataset::factory({0.0,  9.0, 9.1, 10.9, 11.0, 15.0, 16.0,19.0, 19.1, 20.9, 21.0,  25.0, 26.0, 29.0, 29.1, 30.9, 31.0, 40.0},
                                                            {1.0, 1.0, 1.0,1.0, 1.0,1.0, 2.0,  2.0, 2.0,  2.0, 2.0,  2.0,  3.0,  3.0,  3.0,  3.0,  3.0,  3.0});
  XYDataset::XYDataset xys2 = XYDataset::XYDataset::factory({0.0,  9.0, 9.1, 10.9, 11.0, 15.0, 16.0,19.0, 19.1, 20.9, 21.0,  25.0, 26.0, 29.0, 29.1, 30.9, 31.0, 40.0},
                                                            {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 2.0,  2.0,  2.0,  2.0, 2.0,  2.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0});
  std::map<Euclid::XYDataset::QualifiedName, XYDataset::XYDataset> storage_sed;
  storage_sed.insert(std::make_pair(s1, xys1));
  storage_sed.insert(std::make_pair(s2, xys2));
  auto sed_provider = std::shared_ptr<XYDataset::XYDatasetProvider>{new MockDatasetProvider{std::move(storage_sed)}};
  std::set<XYDataset::QualifiedName> sed_list {s1, s2};

 std::vector<std::vector<double>> colors = computer.computeSedColors(ordered, sed_list, sed_provider, filter_provider);

 BOOST_CHECK_EQUAL(2, colors.size());
 BOOST_CHECK_EQUAL(2, colors[0].size());



 BOOST_CHECK_CLOSE(-0.752575, colors[0][0], 0.05);
 BOOST_CHECK_CLOSE(-0.4402282, colors[0][1], 0.05);

 BOOST_CHECK_CLOSE(0.4402282, colors[1][0], 0.05);
 BOOST_CHECK_CLOSE(0.752575, colors[1][1], 0.05);

}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


