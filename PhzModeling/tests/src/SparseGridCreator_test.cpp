/**
 * Copyright (C) 2022 Euclid Science Ground Segment
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

#include "PhzModeling/SparseGridCreator.h"
#include <AlexandriaKernel/memory_tools.h>
#include <boost/test/unit_test.hpp>
#include <cfenv>

using namespace Euclid::PhzDataModel;

using Euclid::make_unique;
using Euclid::GridContainer::GridAxis;
using Euclid::PhzModeling::SparseGridCreator;
using Euclid::XYDataset::QualifiedName;
using Euclid::XYDataset::XYDataset;

class MockProvider : public Euclid::XYDataset::XYDatasetProvider {
public:
  using map_t = std::map<std::string, XYDataset>;

  ~MockProvider() override = default;

  explicit MockProvider(map_t contents) : m_content(std::move(contents)) {}

  std::vector<QualifiedName> listContents(const std::string&) override {
    // Unused
    return {};
  }

  std::string getParameter(const Euclid::XYDataset::QualifiedName&, const std::string& key_word) override {
    if (key_word == "FilterType") {
      return "energy";
    }
    return "";
  }

  std::unique_ptr<XYDataset> getDataset(const Euclid::XYDataset::QualifiedName& qualified_name) override {
    return make_unique<XYDataset>(m_content.at(qualified_name.qualifiedName()));
  }

private:
  map_t m_content;
};

//----------------------------------------------------------------------------

XYDataset MockIGM(const XYDataset& data, double) {
  return data;
}

//----------------------------------------------------------------------------

XYDataset MockNorm(const XYDataset& data) {
  return data;
}

//----------------------------------------------------------------------------

struct SparseGridCreatorFixture {
  XYDataset           sed            = XYDataset::factory({1, 10, 20}, {0.0, 1.0, 0.0});
  XYDataset           red            = XYDataset::factory({1, 10, 20}, {0.5, 0.3, 0.1});
  XYDataset           filter         = XYDataset::factory({1, 10, 20}, {1., 0.5, 1.});
  std::vector<double> expected_photo = {0.0, 0.35, 0.0};
};

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SparseGridCreator_test)

//----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(createGrid_test, SparseGridCreatorFixture) {
  auto sed_provider    = std::make_shared<MockProvider>(MockProvider::map_t{{"SED1", sed}, {"SED2", sed}});
  auto red_provider    = std::make_shared<MockProvider>(MockProvider::map_t{{"RED1", red}, {"RED2", red}});
  auto filter_provider = std::make_shared<MockProvider>(MockProvider::map_t{{"F1", filter}, {"F2", filter}});

  GridAxis<double>        z_axis{"Z", {0., 1., 2.}};
  GridAxis<double>        ebv_axis{"EBV", {0., 0.5, 0.7}};
  GridAxis<QualifiedName> red_axis{"RED", {QualifiedName{"RED1"}, QualifiedName{"RED2"}}};
  GridAxis<QualifiedName> sed_axis{"SED", {QualifiedName{"SED1"}, QualifiedName{"SED2"}}};
  ModelAxesTuple          axes{z_axis, ebv_axis, red_axis, sed_axis};

  size_t last_step  = 0;
  size_t last_total = 0;
  auto   progress   = [&last_step, &last_total](size_t step, size_t total) {
    last_step  = step;
    last_total = total;
  };

  SparseGridCreator grid_creator(sed_provider, red_provider, filter_provider, MockIGM, MockNorm);
  auto              grid_map = grid_creator.createGrid({{"REGION1", axes}}, {QualifiedName{"F1"}, QualifiedName{"F2"}},
                                                       Euclid::PhysicsUtils::CosmologicalParameters{}, progress);

  BOOST_CHECK_EQUAL(grid_map.size(), 1);

  const auto& grid = grid_map.at("REGION1");

  BOOST_CHECK_EQUAL(grid.size(), 36);
  BOOST_CHECK_EQUAL(last_total, 36);
  BOOST_CHECK_EQUAL(last_step, last_total);

  for (auto& photo : grid) {
    for (auto& filter : photo) {
      BOOST_CHECK_GT(filter.flux, 0.);
    }
  }
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//----------------------------------------------------------------------------
