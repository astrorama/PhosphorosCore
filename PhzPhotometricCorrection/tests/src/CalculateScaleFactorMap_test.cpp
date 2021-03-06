/**
 * @file tests/src/CalculateScaleFactorMap_test.cpp
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#include <boost/test/unit_test.hpp>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "ScaleFactorCalcMockForMap.h"
#include "SourceCatalog/Catalog.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

using namespace std::placeholders;

struct CalculateScaleFactorMap_Fixture {

  std::shared_ptr<std::vector<std::string>> filters;

  std::vector<SourceCatalog::Source> m_catalog{};

  std::map<SourceCatalog::Source::id_type, SourceCatalog::Photometry> model_map{};

  CalculateScaleFactorMap_Fixture() {
    // define the filters list
    filters =
        std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter_1", "filter_2", "filter_3"});

    int64_t source_1_id = 1;
    // build the catalog
    auto value_photometry_1 = std::vector<SourceCatalog::FluxErrorPair>{{1.1, 0.1}, {1.2, 0.2}, {1.3, 0.3}};
    auto attribute_1        = std::vector<std::shared_ptr<SourceCatalog::Attribute>>();

    attribute_1.push_back(std::shared_ptr<SourceCatalog::Attribute>(
        new SourceCatalog::Photometry(filters, std::move(value_photometry_1))));

    auto source_1 = SourceCatalog::Source(source_1_id, std::move(attribute_1));
    m_catalog.push_back(std::move(source_1));

    int64_t source_2_id        = 2;
    auto    value_photometry_2 = std::vector<SourceCatalog::FluxErrorPair>{{2.1, 0.1}, {2.2, 0.2}, {2.3, 0.3}};
    auto    attribute_2        = std::vector<std::shared_ptr<SourceCatalog::Attribute>>();

    attribute_2.push_back(std::shared_ptr<SourceCatalog::Attribute>(
        new SourceCatalog::Photometry(filters, std::move(value_photometry_2))));
    auto source_2 = SourceCatalog::Source(source_2_id, std::move(attribute_2));
    m_catalog.push_back(std::move(source_2));

    auto model_photo_Value_1 = std::vector<SourceCatalog::FluxErrorPair>{{10.1, 0.1}, {10.2, 0.2}, {10.3, 0.3}};
    auto model_phot_1        = SourceCatalog::Photometry(filters, std::move(model_photo_Value_1));
    auto model_photo_Value_2 = std::vector<SourceCatalog::FluxErrorPair>{{20.1, 0.1}, {20.2, 0.2}, {20.3, 0.3}};
    auto model_phot_2        = SourceCatalog::Photometry(filters, std::move(model_photo_Value_2));

    model_map.emplace(source_1_id, model_phot_1);
    model_map.emplace(source_2_id, model_phot_2);
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CalculateScaleFactorMap_test)
BOOST_FIXTURE_TEST_CASE(Functional_call_test, CalculateScaleFactorMap_Fixture) {

  ScaleFactorCalcMockForMap functorMock;

  {
    InSequence s;
    EXPECT_CALL(functorMock, FCall(_, _, _))
        .With(AllOf(Args<0>(Truly([](std::tuple<SourceCatalog::Photometry::const_iterator> args) {
                      BOOST_CHECK(Elements::isEqual((*std::get<0>(args)).flux, 1.1));
                      // std::cout<<(*std::get<0>(args)).flux;
                      return true;
                    })),
                    Args<2>(Truly([](std::tuple<SourceCatalog::Photometry::const_iterator> args) {
                      BOOST_CHECK(Elements::isEqual((*std::get<0>(args)).flux, 10.1));
                      // std::cout<<(*std::get<0>(args)).flux;
                      return true;
                    }))))
        .WillOnce(Return(1));

    EXPECT_CALL(functorMock, FCall(_, _, _))
        .With(AllOf(Args<0>(Truly([](std::tuple<SourceCatalog::Photometry::const_iterator> args) {
                      BOOST_CHECK(Elements::isEqual((*std::get<0>(args)).flux, 2.1));
                      // std::cout<<(*std::get<0>(args)).flux;
                      return true;
                    })),
                    Args<2>(Truly([](std::tuple<SourceCatalog::Photometry::const_iterator> args) {
                      BOOST_CHECK(Elements::isEqual((*std::get<0>(args)).flux, 20.1));
                      //  std::cout<<(*std::get<0>(args)).flux;
                      return true;
                    }))))
        .WillOnce(Return(2));
  }

  auto functor = CalculateScaleFactorMap(functorMock.getFunctorObject());
  auto scale   = functor(m_catalog.begin(), m_catalog.end(), model_map);
}

BOOST_AUTO_TEST_SUITE_END()

}  // end of namespace PhzPhotometricCorrection
}  // end of namespace Euclid
