/**
 * @file tests/src/lib/SedClassifier_test.cpp
 * @date 2019-03-19
 * @author Florian dubath
 */

#include "PhzNzPrior/SedClassifier.h"

#include <boost/test/unit_test.hpp>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SedClassifier_test)

struct MockProvider : public XYDataset::XYDatasetProvider {
  std::map<std::string, std::vector<XYDataset::QualifiedName>> m_listing;
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> m_dataset;
  std::vector<double> m_x = {0., 1., 2., 3.,  4.,  5., 6.,
                             7., 8., 9., 10., 11., 12.};
  std::vector<double> m_B_y = {0., 0., 1., 1., 1., 0., 0.,
                               0., 0., 0., 0., 0., 0.};
  std::vector<double> m_I_y = {0., 0., 0., 0., 0., 0., 0.,
                               0., 1., 1., 1., 0., 0.};
  std::vector<double> m_I_S0 = {0., 1., 1., 1., 1., 1., 2.,
                                2., 2., 2., 2., 2., 0.};
  std::vector<double> m_I_S1 = {0., 1., 1., 1., 1., 1., 1.,
                                1., 1., 1., 1., 1., 0.};
  std::vector<double> m_I_S2 = {0., 2., 2., 2., 2., 2., 2.,
                                1., 1., 1., 1., 1., 0.};

  virtual ~MockProvider() = default;

  MockProvider() {
    m_listing["F"] = {{"B"}, {"I"}};
    m_listing["S"] = {{"1"}, {"2"}, {"3"}};

    m_dataset.emplace(XYDataset::QualifiedName{"F/B"},
                      std::move(XYDataset::XYDataset::factory(m_x, m_B_y)));
    m_dataset.emplace(XYDataset::QualifiedName{"F/I"},
                      std::move(XYDataset::XYDataset::factory(m_x, m_I_y)));
    m_dataset.emplace(XYDataset::QualifiedName{"S/0"},
                      std::move(XYDataset::XYDataset::factory(m_x, m_I_S0)));
    m_dataset.emplace(XYDataset::QualifiedName{"S/1"},
                      std::move(XYDataset::XYDataset::factory(m_x, m_I_S1)));
    m_dataset.emplace(XYDataset::QualifiedName{"S/2"},
                      std::move(XYDataset::XYDataset::factory(m_x, m_I_S2)));
  }

  std::vector<XYDataset::QualifiedName> listContents(
      const std::string& group) override {
    auto i = m_listing.find(group);
    if (i == m_listing.end()) {
      return {};
    }
    return i->second;
  }

  std::unique_ptr<XYDataset::XYDataset> getDataset(
      const XYDataset::QualifiedName& qualified_name) override {
    auto i = m_dataset.find(qualified_name);
    if (i == m_dataset.end()) {
      return nullptr;
    }
    return std::unique_ptr<XYDataset::XYDataset>{
        new XYDataset::XYDataset{i->second}};
  }

  std::string getParameter(const XYDataset::QualifiedName& /* qualified_name */,
                           const std::string& key_word) override {
    return key_word;
  }
};

struct SedClassifier_fixture {
  std::shared_ptr<MockProvider> mock_provider{new MockProvider{}};
  XYDataset::QualifiedName filter_B = XYDataset::QualifiedName{"F/B"};
  XYDataset::QualifiedName filter_I = XYDataset::QualifiedName{"F/I"};
  XYDataset::QualifiedName SED_0 = XYDataset::QualifiedName{"S/0"};
  XYDataset::QualifiedName SED_1 = XYDataset::QualifiedName{"S/1"};
  XYDataset::QualifiedName SED_2 = XYDataset::QualifiedName{"S/2"};
};

/**
 * Check the constructor
 */
BOOST_FIXTURE_TEST_CASE(Constructor_test, SedClassifier_fixture) {
  BOOST_CHECK_NO_THROW(PhzNzPrior::SedClassifier(mock_provider, mock_provider));
  BOOST_CHECK_NO_THROW(
      PhzNzPrior::SedClassifier(mock_provider, mock_provider, 1, 2));

  // Inverted or equals parameters
  BOOST_CHECK_THROW(
      PhzNzPrior::SedClassifier(mock_provider, mock_provider, 2, 1),
      Elements::Exception);
  BOOST_CHECK_THROW(
      PhzNzPrior::SedClassifier(mock_provider, mock_provider, 1, 1),
      Elements::Exception);
}

/**
 * Check SED disjoint from filter
 */
BOOST_FIXTURE_TEST_CASE(emptyFilter_test, SedClassifier_fixture) {
  auto classifier = PhzNzPrior::SedClassifier(mock_provider, mock_provider);

  auto SED_list = std::vector<XYDataset::QualifiedName>{filter_B};
  BOOST_CHECK_THROW(classifier(filter_B, filter_I, SED_list),
                    Elements::Exception);

  SED_list = std::vector<XYDataset::QualifiedName>{SED_0};
  BOOST_CHECK_NO_THROW(classifier(filter_B, filter_I, SED_list));
}

BOOST_FIXTURE_TEST_CASE(nominal_case_test, SedClassifier_fixture) {
  auto classifier =
      PhzNzPrior::SedClassifier(mock_provider, mock_provider, 1., 2.);

  auto SED_list = std::vector<XYDataset::QualifiedName>{SED_0, SED_1, SED_2};
  auto result = classifier(filter_B, filter_I, SED_list);

  auto groups = result.getManagedGroups();

  // return 3 groups
  BOOST_CHECK_EQUAL(groups.size(), 3);

  // with names T1, T2, T3
  std::string key = "T1";
  BOOST_CHECK(groups.find(key) != groups.end());
  key = "T2";
  BOOST_CHECK(groups.find(key) != groups.end());
  key = "T3";
  BOOST_CHECK(groups.find(key) != groups.end());

  // check the content
  BOOST_CHECK_EQUAL(result.findGroupContaining(SED_0).first, "T1");
  BOOST_CHECK_EQUAL(result.findGroupContaining(SED_1).first, "T2");
  BOOST_CHECK_EQUAL(result.findGroupContaining(SED_2).first, "T3");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
