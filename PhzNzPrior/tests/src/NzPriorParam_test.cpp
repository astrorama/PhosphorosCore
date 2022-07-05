/**
 * @file tests/src/lib/NzPriorParam_test.cpp
 * @date 2019-03-19
 * @author Florian dubath
 */

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Real.h"
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "PhzNzPrior/NzPriorParam.h"

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(NzPriorParam_test)

/**
 * Check the Luminosity in flux
 */
BOOST_AUTO_TEST_CASE(Const_and_accessor_test) {
  auto param = PhzNzPrior::NzPriorParam(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  BOOST_CHECK_CLOSE(1, param.getZ0t(1), 0.00001);
  BOOST_CHECK_CLOSE(2, param.getKmt(1), 0.00001);
  BOOST_CHECK_CLOSE(3, param.getAlphat(1), 0.00001);
  BOOST_CHECK_CLOSE(4, param.getKt(1), 0.00001);
  BOOST_CHECK_CLOSE(5, param.getFt(1), 0.00001);
  BOOST_CHECK_CLOSE(6, param.getCst(1), 0.00001);

  BOOST_CHECK_CLOSE(7, param.getZ0t(2), 0.00001);
  BOOST_CHECK_CLOSE(8, param.getKmt(2), 0.00001);
  BOOST_CHECK_CLOSE(9, param.getAlphat(2), 0.00001);
  BOOST_CHECK_CLOSE(10, param.getKt(2), 0.00001);
  BOOST_CHECK_CLOSE(11, param.getFt(2), 0.00001);
  BOOST_CHECK_CLOSE(12, param.getCst(2), 0.00001);

  BOOST_CHECK_CLOSE(13, param.getZ0t(3), 0.00001);
  BOOST_CHECK_CLOSE(14, param.getKmt(3), 0.00001);
  BOOST_CHECK_CLOSE(15, param.getAlphat(3), 0.00001);
  BOOST_CHECK_CLOSE(16, param.getCst(3), 0.00001);
}

BOOST_AUTO_TEST_CASE(default_test) {
  auto param = PhzNzPrior::NzPriorParam::defaultParam();

  BOOST_CHECK_CLOSE(0.431, param.getZ0t(1), 0.00001);
  BOOST_CHECK_CLOSE(0.091, param.getKmt(1), 0.00001);
  BOOST_CHECK_CLOSE(2.46, param.getAlphat(1), 0.00001);
  BOOST_CHECK_CLOSE(0.4, param.getKt(1), 0.00001);
  BOOST_CHECK_CLOSE(0.30, param.getFt(1), 0.00001);
  BOOST_CHECK_CLOSE(0.8869, param.getCst(1), 0.00001);

  BOOST_CHECK_CLOSE(0.390, param.getZ0t(2), 0.00001);
  BOOST_CHECK_CLOSE(0.100, param.getKmt(2), 0.00001);
  BOOST_CHECK_CLOSE(1.81, param.getAlphat(2), 0.00001);
  BOOST_CHECK_CLOSE(0.3, param.getKt(2), 0.00001);
  BOOST_CHECK_CLOSE(0.35, param.getFt(2), 0.00001);
  BOOST_CHECK_CLOSE(0.8891, param.getCst(2), 0.00001);

  BOOST_CHECK_CLOSE(0.300, param.getZ0t(3), 0.00001);
  BOOST_CHECK_CLOSE(0.150, param.getKmt(3), 0.00001);
  BOOST_CHECK_CLOSE(2.00, param.getAlphat(3), 0.00001);
  BOOST_CHECK_CLOSE(0.8874, param.getCst(3), 0.00001);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
