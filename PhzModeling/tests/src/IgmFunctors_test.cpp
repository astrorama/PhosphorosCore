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

#include "PhzModeling/InoueIgmFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

using Euclid::PhzModeling::InoueIgmFunctor;
using Euclid::PhzModeling::MadauIgmFunctor;
using Euclid::PhzModeling::MeiksinIgmFunctor;
using Euclid::XYDataset::XYDataset;

struct IgmFixture {
  XYDataset sed{
      XYDataset::factory({300, 500, 1000, 1500, 2000, 5000, 10000}, {0.01, 0.03, 0.04, 0.03, 0.05, 0.001, 0.0001})};
  double z{2.5};
};

using IgmFunctors = boost::mpl::list<InoueIgmFunctor, MadauIgmFunctor, MeiksinIgmFunctor>;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(IgmFunctors_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Igm_test, IFunctor, IgmFunctors, IgmFixture) {
  IFunctor  igm_functor{};
  XYDataset reddened = igm_functor(sed, z);
  for (auto i1 = reddened.begin(), i2 = sed.begin(); i1 != reddened.end(); ++i1, ++i2) {
    BOOST_CHECK_EQUAL(i1->first, i2->first);
    // Blue side must be reddened
    if (i1->first <= 2000) {
      BOOST_CHECK_LT(i1->second, i2->second);
    }
    // Red side is not affected
    else {
      BOOST_CHECK_CLOSE(i1->second, i2->second, 1e-6);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------