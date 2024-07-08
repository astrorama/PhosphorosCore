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

#include "PhzModeling/InoueCgmIgmFunctor.h"
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

using Euclid::XYDataset::XYDataset;

struct IgmFixture {
  XYDataset sed{
      XYDataset::factory({300, 500, 1000, 1500, 2000, 5000, 10000}, {0.01, 0.03, 0.04, 0.03, 0.05, 0.001, 0.0001})};
  double z{2.5};
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(IgmInoueCmgFunctors_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(sigmoid_test, IgmFixture) {
  std::vector<double> z{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  std::vector<double> expected{17.599454325421615, 17.655575741288818, 17.77170576145825,  18.00286662798902,
                               18.429418447684405, 19.116675778763888, 20.013956565,       20.911237351236114,
                               21.598494682315597, 22.02504650201098,  22.25620736854175,  22.372337388711188,
                               22.428458804578387, 22.455071571095026, 22.467577985189465, 22.473430326691734};

  BOOST_CHECK_CLOSE(Euclid::PhzModeling::InoueCgmIgmFunctor::InoueCgmIgmFunctor::sigmoid(0, 1, 1, 0),
                    0.0024726231566347743, 1e-6);

  for (size_t index = 0; index < z.size(); ++index) {
    double value = Euclid::PhzModeling::InoueCgmIgmFunctor::InoueCgmIgmFunctor::sigmoid(z[index], 4.92919285,
                                                                                        0.76313514, 17.54936014);
    BOOST_CHECK_CLOSE(value, expected[index], 1e-6);
  }
}

BOOST_FIXTURE_TEST_CASE(sigma_alpha, IgmFixture) {
  std::vector<double> nu{0, 1000, 1500, 2000, 4000, 6000, 8000, 10000};
  std::vector<double> expected{0.0,
                               7.773968332535842e-76,
                               3.9355714683478664e-75,
                               1.2438349332067436e-74,
                               1.9901358931340173e-73,
                               1.0075062959007306e-72,
                               3.1842174290247575e-72,
                               7.773968332592582e-72};

  for (size_t index = 0; index < nu.size(); ++index) {
    double value = Euclid::PhzModeling::InoueCgmIgmFunctor::InoueCgmIgmFunctor::sigma_alpha(nu[index], 6.255486e8,
                                                                                            2.46607e15, 6.9029528e22);
    BOOST_CHECK_CLOSE(value, expected[index], 1e-6);
  }
}

BOOST_FIXTURE_TEST_CASE(buildCmgIgmTransmissionFunc, IgmFixture) {
  auto                funct = Euclid::PhzModeling::InoueCgmIgmFunctor::buildCmgIgmTransmissionFunc(7);
  std::vector<double> lam{1000, 1500, 2000, 4000, 6000, 8000, 10000};
  std::vector<double> expected{0.99724646, 0.99862208, 0.99912122, 0.99960015, 0.99958079, 0.99890562, 0.97113428};
  for (size_t index = 0; index < lam.size(); ++index) {
    double value = (*funct)(lam[index]);
    BOOST_CHECK_CLOSE(value, expected[index], 1e-6);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
