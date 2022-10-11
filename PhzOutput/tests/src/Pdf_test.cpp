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

#include "PhzOutput/PhzColumnHandlers/Pdf.h"
#include <boost/test/unit_test.hpp>
#include <random>

using namespace Euclid::PhzDataModel;

using Euclid::GridContainer::GridAxis;
using Euclid::PhzDataModel::Pdf1DParam;
using Euclid::PhzOutput::ColumnHandlers::Pdf;
using Euclid::SourceCatalog::Source;
using Euclid::XYDataset::QualifiedName;

struct PdfFixture {
  Source                  source{12345, {}};
  SourceResults           results;
  GridAxis<double>        z_axis{"Z", {0., 1., 2.}};
  GridAxis<double>        ebv_axis{"EBV", {0., 0.5, 0.7}};
  GridAxis<QualifiedName> red_axis{"RED", {QualifiedName{"CURVE1"}, QualifiedName{"CURVE2"}, QualifiedName{"CURVE3"}}};
  GridAxis<QualifiedName> sed_axis{"SED", {QualifiedName{"SED1"}, QualifiedName{"SED2"}, QualifiedName{"SED3"}}};
  Pdf1DParam<ModelParameter::Z>               pdf_z{z_axis};
  Pdf1DParam<ModelParameter::EBV>             pdf_ebv{ebv_axis};
  Pdf1DParam<ModelParameter::REDDENING_CURVE> pdf_red{red_axis};
  Pdf1DParam<ModelParameter::REDDENING_CURVE> pdf_sed{sed_axis};

  PdfFixture() {
    std::default_random_engine         engine(0);
    std::uniform_int_distribution<int> dist(0, 100);

    auto generator = [&dist, &engine]() {
      return dist(engine);
    };
    std::generate_n(pdf_z.begin(), pdf_z.size(), generator);
    std::generate_n(pdf_ebv.begin(), pdf_ebv.size(), generator);
    std::generate_n(pdf_red.begin(), pdf_red.size(), generator);
    std::generate_n(pdf_sed.begin(), pdf_sed.size(), generator);

    results.set<SourceResultType::Z_1D_PDF>(pdf_z.copy());
    results.set<SourceResultType::RED_CURVE_1D_PDF>(pdf_red.copy());
    results.set<SourceResultType::EBV_1D_PDF>(pdf_ebv.copy());
    results.set<SourceResultType::SED_1D_PDF>(pdf_sed.copy());

    results.set<SourceResultType::LIKELIHOOD_Z_1D_PDF>(pdf_z.copy());
    results.set<SourceResultType::LIKELIHOOD_RED_CURVE_1D_PDF>(pdf_red.copy());
    results.set<SourceResultType::LIKELIHOOD_EBV_1D_PDF>(pdf_ebv.copy());
    results.set<SourceResultType::LIKELIHOOD_SED_1D_PDF>(pdf_sed.copy());
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Pdf_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGridZ, PdfFixture) {
  Pdf<GridType::POSTERIOR, ModelParameter::Z> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "Z-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_z.begin(), pdf_z.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGridReddening, PdfFixture) {
  Pdf<GridType::POSTERIOR, ModelParameter::REDDENING_CURVE> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "REDDENING-CURVE-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_red.begin(), pdf_red.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGridEBV, PdfFixture) {
  Pdf<GridType::POSTERIOR, ModelParameter::EBV> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "EBV-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_ebv.begin(), pdf_ebv.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_PosteriorGridSed, PdfFixture) {
  Pdf<GridType::POSTERIOR, ModelParameter::SED> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "SED-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_sed.begin(), pdf_sed.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGridZ, PdfFixture) {
  Pdf<GridType::LIKELIHOOD, ModelParameter::Z> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "LIKELIHOOD-Z-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_z.begin(), pdf_z.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGridReddening, PdfFixture) {
  Pdf<GridType::LIKELIHOOD, ModelParameter::REDDENING_CURVE> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "LIKELIHOOD-REDDENING-CURVE-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_red.begin(), pdf_red.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGridEBV, PdfFixture) {
  Pdf<GridType::LIKELIHOOD, ModelParameter::EBV> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "LIKELIHOOD-EBV-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_ebv.begin(), pdf_ebv.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(test_LikelihoodGridSed, PdfFixture) {
  Pdf<GridType::LIKELIHOOD, ModelParameter::SED> pdf{};

  auto column_info = pdf.getColumnInfoList();
  BOOST_CHECK_EQUAL(column_info.size(), 1);
  BOOST_CHECK_EQUAL(column_info.at(0).name, "LIKELIHOOD-SED-1D-PDF");

  auto row = pdf.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 1);
  auto& cell_val = boost::get<std::vector<double>>(row.at(0));
  BOOST_CHECK_EQUAL_COLLECTIONS(pdf_sed.begin(), pdf_sed.end(), cell_val.begin(), cell_val.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------