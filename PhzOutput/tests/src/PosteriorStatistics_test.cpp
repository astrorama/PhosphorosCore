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

#include "PhzOutput/PhzColumnHandlers/PosteriorStatistics.h"
#include <boost/test/unit_test.hpp>

using Euclid::PhzDataModel::SourceResults;
using Euclid::PhzDataModel::SourceResultType;
using Euclid::PhzOutput::ColumnHandlers::PosteriorStatistics;
using Euclid::SourceCatalog::Source;

using PDZBins = Euclid::GridContainer::GridAxis<double>;
using PDZ     = Euclid::PhzDataModel::Pdf1DParam<Euclid::PhzDataModel::ModelParameter::Z>;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PosteriorStatistics_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_Output) {
  PosteriorStatistics posterior_statistics;

  auto columns_info = posterior_statistics.getColumnInfoList();
  BOOST_CHECK_EQUAL(columns_info.size(), 3);
  BOOST_CHECK_EQUAL(columns_info.at(0).name, "Posterior-Log");
  BOOST_CHECK_EQUAL(columns_info.at(1).name, "Likelihood-Log");
  BOOST_CHECK_EQUAL(columns_info.at(2).name, "1DPDF-Peak-Z");

  Source              source{1234, {}};
  SourceResults       results;
  PDZBins             pdz_bins{"Z", {0., 1., 2., 3.}};
  std::vector<double> pdz_vals{0.1, 0.2, 5.0, 0.0};
  PDZ                 pdz{std::make_tuple(pdz_bins)};
  std::copy(pdz_vals.begin(), pdz_vals.end(), pdz.begin());

  results.set<SourceResultType::BEST_MODEL_LIKELIHOOD_LOG>(0.5);
  results.set<SourceResultType::BEST_MODEL_POSTERIOR_LOG>(0.1);
  results.set<SourceResultType::Z_1D_PDF>(std::move(pdz));

  auto row = posterior_statistics.convertResults(source, results);
  BOOST_CHECK_EQUAL(row.size(), 3);

  BOOST_CHECK_EQUAL(boost::get<double>(row.at(0)), 0.1);
  BOOST_CHECK_EQUAL(boost::get<double>(row.at(1)), 0.5);
  BOOST_CHECK_EQUAL(boost::get<double>(row.at(2)), 2.0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
