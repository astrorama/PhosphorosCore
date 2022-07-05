/**
 * @file FindMedianPhotometricCorrectionsFunctor_test.cpp
 * @date January 23, 2015
 * @author Florian Dubath
 */

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzPhotometricCorrection/FindMedianPhotometricCorrectionsFunctor.h"
#include "SourceCatalog/Source.h"
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <map>
#include <memory>
#include <vector>

using std::initializer_list;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;
using namespace Euclid;
using namespace Euclid::SourceCatalog;

struct FindMedianPhotometricCorrectionsFunctor_Fixture {

  std::map<Source::id_type, PhzDataModel::PhotometricCorrectionMap> source_phot_corr_map{
      {1, {{XYDataset::QualifiedName{"Filter_1"}, 1}, {XYDataset::QualifiedName{"Filter_2"}, 11}}},
      {2, {{XYDataset::QualifiedName{"Filter_1"}, 3}, {XYDataset::QualifiedName{"Filter_2"}, 10}}},
      {3, {{XYDataset::QualifiedName{"Filter_1"}, 5}, {XYDataset::QualifiedName{"Filter_2"}, 8}}},
      {4, {{XYDataset::QualifiedName{"Filter_1"}, 7}, {XYDataset::QualifiedName{"Filter_2"}, 12}}},
      {5, {{XYDataset::QualifiedName{"Filter_1"}, 101}, {XYDataset::QualifiedName{"Filter_2"}, 20}}}};

  std::map<Source::id_type, PhzDataModel::PhotometricCorrectionMap> source_missing_phot_corr_map{
      {1,
       {{XYDataset::QualifiedName{"Filter_1"}, 1},
        {XYDataset::QualifiedName{"Filter_2"}, NAN},
        {XYDataset::QualifiedName{"Filter_3"}, NAN}}},
      {2,
       {{XYDataset::QualifiedName{"Filter_1"}, 3},
        {XYDataset::QualifiedName{"Filter_2"}, NAN},
        {XYDataset::QualifiedName{"Filter_3"}, NAN}}},
      {3,
       {{XYDataset::QualifiedName{"Filter_1"}, 5},
        {XYDataset::QualifiedName{"Filter_2"}, NAN},
        {XYDataset::QualifiedName{"Filter_3"}, 102}}},
      {4,
       {{XYDataset::QualifiedName{"Filter_1"}, 7},
        {XYDataset::QualifiedName{"Filter_2"}, NAN},
        {XYDataset::QualifiedName{"Filter_3"}, 103}}},
      {5,
       {{XYDataset::QualifiedName{"Filter_1"}, 101},
        {XYDataset::QualifiedName{"Filter_2"}, NAN},
        {XYDataset::QualifiedName{"Filter_3"}, 104}}}};

  vector<Source> sources{
      {1,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{1., 1.}, {11., 11.}, {100., 1.}, {100., 100.}}}}}},
      {2,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{3., 3.}, {10., 10.}, {101., 100.}, {101., 100.}}}}}},
      {3,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{5., 5.}, {8., 8.}, {102., 100.}, {102., 100.}}}}}},
      {4,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{7., 7.}, {12., 4.}, {103., 100.}, {103., 100.}}}}}},
      {5,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{101., 101.}, {20., 5.}, {104., 100.}, {104., 1.}}}}}}};

  vector<Source> sources_missing{
      {1,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{1., 1.}, {11., 11., true, false}, {100., 1., true}, {100., 100.}}}}}},
      {2,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{3., 3.}, {10., 10., true, false}, {101., 100., true}, {101., 100.}}}}}},
      {3,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{5., 5.}, {8., 8., true, false}, {102., 100.}, {102., 100.}}}}}},
      {4,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{7., 7.}, {12., 4., false, true}, {103., 100.}, {103., 100.}}}}}},
      {5,
       {shared_ptr<Attribute>{new Photometry{
           make_shared<vector<string>>(initializer_list<string>{"Filter_1", "Filter_2", "Filter_3", "Filter_4"}),
           vector<FluxErrorPair>{{101., 101.}, {20., 5., false, true}, {104., 100.}, {104., 1.}}}}}}};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FindMedianPhotometricCorrectionsFunctor_test)

//-----------------------------------------------------------------------------
// Check the functor returns the median value also for unordered inputs
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NoInputSources_test, FindMedianPhotometricCorrectionsFunctor_Fixture) {
  PhzPhotometricCorrection::FindMedianPhotometricCorrectionsFunctor functor{};
  auto result = functor(source_phot_corr_map, sources.begin(), sources.end());

  BOOST_CHECK(Elements::isEqual(5., result.at({"Filter_1"})));
  BOOST_CHECK(Elements::isEqual(11., result.at({"Filter_2"})));
}

//-----------------------------------------------------------------------------
// Check the functor returns 1 if all values are missing/upper limit
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(MissingPhotSources_test, FindMedianPhotometricCorrectionsFunctor_Fixture) {
  PhzPhotometricCorrection::FindMedianPhotometricCorrectionsFunctor functor{};
  auto result = functor(source_missing_phot_corr_map, sources_missing.begin(), sources_missing.end());

  // with equal weight we should recover the median (5).
  BOOST_CHECK(Elements::isEqual(5., result.at({"Filter_1"})));

  // 1
  BOOST_CHECK(Elements::isEqual(1., result.at({"Filter_2"})));
  // partial missing
  BOOST_CHECK(Elements::isEqual(103., result.at({"Filter_3"})));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
