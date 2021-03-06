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

/**
 * @file InoueIgmFunctor.cpp
 * @date May 6, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/InoueIgmFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include <cmath>
#include <limits>
#include <map>
#include <mutex>
#include <vector>

#include <iostream>

namespace Euclid {
namespace PhzModeling {

static const std::vector<double> lyman_lambda{1215.67, 1025.72, 972.537, 949.743, 937.803, 930.748, 926.226, 923.15,
                                              920.963, 919.352, 918.129, 917.181, 916.429, 915.824, 915.329, 914.919,
                                              914.576, 914.286, 914.039, 913.826, 913.641, 913.48,  913.339, 913.215,
                                              913.104, 913.006, 912.918, 912.839, 912.768, 912.703, 912.645, 912.592,
                                              912.543, 912.499, 912.458, 912.42,  912.385, 912.353, 912.324};

static const std::vector<double> a_laf_1{
    1.690e-02, 4.692e-03, 2.239e-03, 1.319e-03, 8.707e-04, 6.178e-04, 4.609e-04, 3.569e-04, 2.843e-04, 2.318e-04,
    1.923e-04, 1.622e-04, 1.385e-04, 1.196e-04, 1.043e-04, 9.174e-05, 8.128e-05, 7.251e-05, 6.505e-05, 5.868e-05,
    5.319e-05, 4.843e-05, 4.427e-05, 4.063e-05, 3.738e-05, 3.454e-05, 3.199e-05, 2.971e-05, 2.766e-05, 2.582e-05,
    2.415e-05, 2.263e-05, 2.126e-05, 2.000e-05, 1.885e-05, 1.779e-05, 1.682e-05, 1.593e-05, 1.510e-05};

static const std::vector<double> a_laf_2{
    2.354e-03, 6.536e-04, 3.119e-04, 1.837e-04, 1.213e-04, 8.606e-05, 6.421e-05, 4.971e-05, 3.960e-05, 3.229e-05,
    2.679e-05, 2.259e-05, 1.929e-05, 1.666e-05, 1.453e-05, 1.278e-05, 1.132e-05, 1.010e-05, 9.062e-06, 8.174e-06,
    7.409e-06, 6.746e-06, 6.167e-06, 5.660e-06, 5.207e-06, 4.811e-06, 4.456e-06, 4.139e-06, 3.853e-06, 3.596e-06,
    3.364e-06, 3.153e-06, 2.961e-06, 2.785e-06, 2.625e-06, 2.479e-06, 2.343e-06, 2.219e-06, 2.103e-06};

static const std::vector<double> a_laf_3{
    1.026e-04, 2.849e-05, 1.360e-05, 8.010e-06, 5.287e-06, 3.752e-06, 2.799e-06, 2.167e-06, 1.726e-06, 1.407e-06,
    1.168e-06, 9.847e-07, 8.410e-07, 7.263e-07, 6.334e-07, 5.571e-07, 4.936e-07, 4.403e-07, 3.950e-07, 3.563e-07,
    3.230e-07, 2.941e-07, 2.689e-07, 2.467e-07, 2.270e-07, 2.097e-07, 1.943e-07, 1.804e-07, 1.680e-07, 1.568e-07,
    1.466e-07, 1.375e-07, 1.291e-07, 1.214e-07, 1.145e-07, 1.080e-07, 1.022e-07, 9.673e-08, 9.169e-08};

static const std::vector<double> a_dla_1{
    1.617e-04, 1.545e-04, 1.498e-04, 1.460e-04, 1.429e-04, 1.402e-04, 1.377e-04, 1.355e-04, 1.335e-04, 1.316e-04,
    1.298e-04, 1.281e-04, 1.265e-04, 1.250e-04, 1.236e-04, 1.222e-04, 1.209e-04, 1.197e-04, 1.185e-04, 1.173e-04,
    1.162e-04, 1.151e-04, 1.140e-04, 1.130e-04, 1.120e-04, 1.110e-04, 1.101e-04, 1.091e-04, 1.082e-04, 1.073e-04,
    1.065e-04, 1.056e-04, 1.048e-04, 1.040e-04, 1.032e-04, 1.024e-04, 1.017e-04, 1.009e-04, 1.002e-04};

static const std::vector<double> a_dla_2{
    5.390e-05, 5.151e-05, 4.992e-05, 4.868e-05, 4.763e-05, 4.672e-05, 4.590e-05, 4.516e-05, 4.448e-05, 4.385e-05,
    4.326e-05, 4.271e-05, 4.218e-05, 4.168e-05, 4.120e-05, 4.075e-05, 4.031e-05, 3.989e-05, 3.949e-05, 3.910e-05,
    3.872e-05, 3.836e-05, 3.800e-05, 3.766e-05, 3.732e-05, 3.700e-05, 3.668e-05, 3.637e-05, 3.607e-05, 3.578e-05,
    3.549e-05, 3.521e-05, 3.493e-05, 3.466e-05, 3.440e-05, 3.414e-05, 3.389e-05, 3.364e-05, 3.339e-05};

// A function which returns the wavelengh for which the absorption has the
// minimum value. These values are precomputed to avoid the costly computations.
std::unique_ptr<MathUtils::Function> min_lambda = MathUtils::interpolate(
    {0.0,
     0.10101010101,
     0.20202020202,
     0.30303030303,
     0.40404040404,
     0.505050505051,
     0.606060606061,
     0.707070707071,
     0.808080808081,
     0.909090909091,
     1.0101010101,
     1.11111111111,
     1.21212121212,
     1.31313131313,
     1.41414141414,
     1.51515151515,
     1.61616161616,
     1.71717171717,
     1.81818181818,
     1.91919191919,
     2.0202020202,
     2.12121212121,
     2.22222222222,
     2.32323232323,
     2.42424242424,
     2.52525252525,
     2.62626262626,
     2.72727272727,
     2.82828282828,
     2.92929292929,
     3.0303030303,
     3.13131313131,
     3.23232323232,
     3.33333333333,
     3.43434343434,
     3.53535353535,
     3.63636363636,
     3.73737373737,
     3.83838383838,
     3.93939393939,
     4.0404040404,
     4.14141414141,
     4.24242424242,
     4.34343434343,
     4.44444444444,
     4.54545454545,
     4.64646464646,
     4.74747474747,
     4.84848484848,
     4.94949494949,
     5.05050505051,
     5.15151515152,
     5.25252525253,
     5.35353535354,
     5.45454545455,
     5.55555555556,
     5.65656565657,
     5.75757575758,
     5.85858585859,
     5.9595959596,
     6.06060606061,
     6.16161616162,
     6.26262626263,
     6.36363636364,
     6.46464646465,
     6.56565656566,
     6.66666666667,
     6.76767676768,
     6.86868686869,
     6.9696969697,
     7.07070707071,
     7.17171717172,
     7.27272727273,
     7.37373737374,
     7.47474747475,
     7.57575757576,
     7.67676767677,
     7.77777777778,
     7.87878787879,
     7.9797979798,
     8.08080808081,
     8.18181818182,
     8.28282828283,
     8.38383838384,
     8.48484848485,
     8.58585858586,
     8.68686868687,
     8.78787878788,
     8.88888888889,
     8.9898989899,
     9.09090909091,
     9.19191919192,
     9.29292929293,
     9.39393939394,
     9.49494949495,
     9.59595959596,
     9.69696969697,
     9.79797979798,
     9.89898989899,
     10.0,
     100.0},
    {430.730730731, 466.166166166, 507.507507508, 542.942942943, 578.378378378, 619.71971972,  655.155155155,
     690.590590591, 726.026026026, 761.461461461, 796.896896897, 832.332332332, 867.767767768, 909.109109109,
     944.544544545, 985.885885886, 1027.22722723, 1074.47447447, 1121.72172172, 1168.96896897, 1228.02802803,
     1287.08708709, 1346.14614615, 1417.01701702, 1493.79379379, 1582.38238238, 1670.97097097, 1777.27727728,
     1895.3953954,  2025.32532533, 2113.91391391, 2190.69069069, 2279.27927928, 2356.05605606, 2426.92692693,
     2503.7037037,  2580.48048048, 2657.25725726, 2769.46946947, 2828.52852853, 2893.49349349, 2958.45845846,
     3023.42342342, 3088.38838839, 3147.44744745, 3212.41241241, 3277.37737738, 3336.43643644, 3401.4014014,
     3466.36636637, 3537.23723724, 3608.10810811, 3684.88488488, 3755.75575576, 3832.53253253, 3915.21521522,
     3991.99199199, 4074.67467467, 4163.26326326, 4245.94594595, 4334.53453453, 4429.02902903, 4517.61761762,
     4612.11211211, 4712.51251251, 4812.91291291, 4913.31331331, 5013.71371371, 5202.7027027,  5232.23223223,
     5320.82082082, 5397.5975976,  5480.28028028, 5568.86886887, 5645.64564565, 5722.42242242, 5799.1991992,
     5893.69369369, 5970.47047047, 6000.0,        6000.0,        6000.0,        6000.0,        6000.0,
     6000.0,        6000.0,        6000.0,        6000.0,        6000.0,        6000.0,        6000.0,
     6000.0,        6000.0,        6000.0,        6000.0,        6000.0,        6000.0,        6000.0,
     6000.0,        6000.0,        6000.0},
    MathUtils::InterpolationType::LINEAR);

// A function which returns the minimum absorption value. These values are
// precomputed to avoid the costly computations.
std::unique_ptr<MathUtils::Function> min_taueff = MathUtils::interpolate({0.0,
                                                                          0.10101010101,
                                                                          0.20202020202,
                                                                          0.30303030303,
                                                                          0.40404040404,
                                                                          0.505050505051,
                                                                          0.606060606061,
                                                                          0.707070707071,
                                                                          0.808080808081,
                                                                          0.909090909091,
                                                                          1.0101010101,
                                                                          1.11111111111,
                                                                          1.21212121212,
                                                                          1.31313131313,
                                                                          1.41414141414,
                                                                          1.51515151515,
                                                                          1.61616161616,
                                                                          1.71717171717,
                                                                          1.81818181818,
                                                                          1.91919191919,
                                                                          2.0202020202,
                                                                          2.12121212121,
                                                                          2.22222222222,
                                                                          2.32323232323,
                                                                          2.42424242424,
                                                                          2.52525252525,
                                                                          2.62626262626,
                                                                          2.72727272727,
                                                                          2.82828282828,
                                                                          2.92929292929,
                                                                          3.0303030303,
                                                                          3.13131313131,
                                                                          3.23232323232,
                                                                          3.33333333333,
                                                                          3.43434343434,
                                                                          3.53535353535,
                                                                          3.63636363636,
                                                                          3.73737373737,
                                                                          3.83838383838,
                                                                          3.93939393939,
                                                                          4.0404040404,
                                                                          4.14141414141,
                                                                          4.24242424242,
                                                                          4.34343434343,
                                                                          4.44444444444,
                                                                          4.54545454545,
                                                                          4.64646464646,
                                                                          4.74747474747,
                                                                          4.84848484848,
                                                                          4.94949494949,
                                                                          5.05050505051,
                                                                          5.15151515152,
                                                                          5.25252525253,
                                                                          5.35353535354,
                                                                          5.45454545455,
                                                                          5.55555555556,
                                                                          5.65656565657,
                                                                          5.75757575758,
                                                                          5.85858585859,
                                                                          5.9595959596,
                                                                          6.06060606061,
                                                                          6.16161616162,
                                                                          6.26262626263,
                                                                          6.36363636364,
                                                                          6.46464646465,
                                                                          6.56565656566,
                                                                          6.66666666667,
                                                                          6.76767676768,
                                                                          6.86868686869,
                                                                          6.9696969697,
                                                                          7.07070707071,
                                                                          7.17171717172,
                                                                          7.27272727273,
                                                                          7.37373737374,
                                                                          7.47474747475,
                                                                          7.57575757576,
                                                                          7.67676767677,
                                                                          7.77777777778,
                                                                          7.87878787879,
                                                                          7.9797979798,
                                                                          8.08080808081,
                                                                          8.18181818182,
                                                                          8.28282828283,
                                                                          8.38383838384,
                                                                          8.48484848485,
                                                                          8.58585858586,
                                                                          8.68686868687,
                                                                          8.78787878788,
                                                                          8.88888888889,
                                                                          8.9898989899,
                                                                          9.09090909091,
                                                                          9.19191919192,
                                                                          9.29292929293,
                                                                          9.39393939394,
                                                                          9.49494949495,
                                                                          9.59595959596,
                                                                          9.69696969697,
                                                                          9.79797979798,
                                                                          9.89898989899,
                                                                          10.0,
                                                                          100.0},
                                                                         {0.851734705391,
                                                                          0.828727577467,
                                                                          0.804764920101,
                                                                          0.779975266333,
                                                                          0.754486291129,
                                                                          0.728414730783,
                                                                          0.701888396295,
                                                                          0.675027269257,
                                                                          0.647947463047,
                                                                          0.620761476975,
                                                                          0.59357748963,
                                                                          0.566498735241,
                                                                          0.539676920502,
                                                                          0.512864843326,
                                                                          0.486063251291,
                                                                          0.45936004987,
                                                                          0.432835808516,
                                                                          0.406570389385,
                                                                          0.380639043734,
                                                                          0.355116109387,
                                                                          0.328925505528,
                                                                          0.303965868998,
                                                                          0.279059311781,
                                                                          0.254369200925,
                                                                          0.230062258436,
                                                                          0.206304545787,
                                                                          0.183258508569,
                                                                          0.161081171387,
                                                                          0.139920851441,
                                                                          0.119841831333,
                                                                          0.101393708315,
                                                                          0.0847810515952,
                                                                          0.0700457179642,
                                                                          0.0571282564266,
                                                                          0.0460077788219,
                                                                          0.0365694761877,
                                                                          0.028675814465,
                                                                          0.022172748029,
                                                                          0.0168105615889,
                                                                          0.0125797582929,
                                                                          0.00927885603668,
                                                                          0.00674345706645,
                                                                          0.00482678268332,
                                                                          0.00340122497333,
                                                                          0.00235837755389,
                                                                          0.00160846968888,
                                                                          0.00107855747107,
                                                                          0.000722003790485,
                                                                          0.000465114008326,
                                                                          0.000292162593789,
                                                                          0.000178678251177,
                                                                          0.000106221399232,
                                                                          6.12766597235e-05,
                                                                          3.42386881022e-05,
                                                                          1.84946673445e-05,
                                                                          9.63788027704e-06,
                                                                          4.83449015267e-06,
                                                                          2.32883519029e-06,
                                                                          1.07461950457e-06,
                                                                          4.73721621269e-07,
                                                                          1.98938248437e-07,
                                                                          7.9345223591e-08,
                                                                          2.99584497854e-08,
                                                                          1.06716421525e-08,
                                                                          3.57325896969e-09,
                                                                          1.12031886117e-09,
                                                                          3.27535689489e-10,
                                                                          8.89038324513e-11,
                                                                          2.16923217191e-11,
                                                                          4.93348411098e-12,
                                                                          1.04415452776e-12,
                                                                          2.03547897853e-13,
                                                                          3.64302068834e-14,
                                                                          5.97306051972e-15,
                                                                          8.92233328624e-16,
                                                                          1.21396135847e-16,
                                                                          1.49891403537e-17,
                                                                          1.66458472961e-18,
                                                                          1.65531275171e-19,
                                                                          1.49087443966e-20,
                                                                          1.25910093352e-21,
                                                                          9.99251686582e-23,
                                                                          7.44572135142e-24,
                                                                          5.20448412711e-25,
                                                                          3.40963504899e-26,
                                                                          2.0917823544e-27,
                                                                          1.20066373589e-28,
                                                                          6.44227602588e-30,
                                                                          3.22838908101e-31,
                                                                          1.50964706482e-32,
                                                                          6.58144501471e-34,
                                                                          2.67260927839e-35,
                                                                          1.01001783334e-36,
                                                                          3.54904745384e-38,
                                                                          1.15849049993e-39,
                                                                          3.50976990021e-41,
                                                                          9.85999714514e-43,
                                                                          2.56621050713e-44,
                                                                          6.18200542907e-46,
                                                                          1.37718178935e-47,
                                                                          0.},
                                                                         MathUtils::InterpolationType::LINEAR);

double t_lyman_series_laf(double z, double l) {
  double t = 0.;
  for (auto l_iter = lyman_lambda.begin(), a1_iter = a_laf_1.begin(), a2_iter = a_laf_2.begin(),
            a3_iter = a_laf_3.begin();
       l_iter != lyman_lambda.end(); ++l_iter, ++a1_iter, ++a2_iter, ++a3_iter) {
    if (l > *l_iter * (1. + z)) {
      return t;
    }
    double l_div = l / *l_iter;
    if (l < 2.2 * *l_iter) {
      t += *a1_iter * std::pow(l_div, 1.2);
    } else if (l < 5.7 * *l_iter) {
      t += *a2_iter * std::pow(l_div, 3.7);
    } else {
      t += *a3_iter * std::pow(l_div, 5.5);
    }
  }
  return t;
}

double t_lyman_series_dla(double z, double l) {
  double t = 0.;
  for (auto l_iter = lyman_lambda.begin(), a1_iter = a_dla_1.begin(), a2_iter = a_dla_2.begin();
       l_iter != lyman_lambda.end(); ++l_iter, ++a1_iter, ++a2_iter) {
    if (l > *l_iter * (1. + z)) {
      return t;
    }
    double l_div = l / *l_iter;
    if (l < 3. * *l_iter) {
      t += *a1_iter * l_div * l_div;
    } else {
      t += *a2_iter * l_div * l_div * l_div;
    }
  }
  return t;
}

double t_lyman_cont_laf(double z, double l) {
  if (l <= 912. * (1. + z)) {
    double l_div = l / 912.;
    if (z < 1.2) {
      return 0.325 * (std::pow(l_div, 1.2) - std::pow(1 + z, -0.9) * std::pow(l_div, 2.1));
    } else if (z < 4.7) {
      if (l < 2.2 * 912.) {
        return 2.55E-2 * std::pow(1 + z, 1.6) * std::pow(l_div, 2.1) + .325 * std::pow(l_div, 1.2) -
               .25 * std::pow(l_div, 2.1);
      } else {
        return 2.55E-2 * (std::pow(1 + z, 1.6) * std::pow(l_div, 2.1) - std::pow(l_div, 3.7));
      }
    } else {
      if (l < 2.2 * 912.) {
        return 5.22E-4 * std::pow(1 + z, 3.4) * std::pow(l_div, 2.1) + .325 * std::pow(l_div, 1.2) -
               3.14E-2 * std::pow(l_div, 2.1);
      } else if (l < 5.7 * 912.) {
        return 5.22E-4 * std::pow(1 + z, 3.4) * std::pow(l_div, 2.1) + .218 * std::pow(l_div, 2.1) -
               2.55E-2 * std::pow(l_div, 3.7);
      } else {
        return 5.22E-4 * (std::pow(1 + z, 3.4) * std::pow(l_div, 2.1) - std::pow(l_div, 5.5));
      }
    }
  }
  return 0.;
}

double t_lyman_cont_dla(double z, double l) {
  if (l <= 912. * (1. + z)) {
    double l_div = l / 912.;
    if (z < 2.) {
      return .211 * std::pow(1 + z, 2) - 7.66E-2 * std::pow(1 + z, 2.3) * std::pow(l_div, -.3) - .135 * l_div * l_div;
    } else {
      if (l < 3. * 912.) {
        return .634 + 4.7E-2 * std::pow(1 + z, 3) - 1.78E-2 * std::pow(1 + z, 3.3) * std::pow(l_div, -.3) -
               .135 * l_div * l_div - .291 * std::pow(l_div, -.3);
      } else {
        return 4.7E-2 * std::pow(1 + z, 3) - 1.78E-2 * std::pow(1 + z, 3.3) * std::pow(l_div, -.3) -
               2.92E-2 * std::pow(l_div, 3);
      }
    }
  }
  return 0.;
}

static std::unique_ptr<MathUtils::Function> buildIgmTransmissionFunc(double z) {
  std::vector<double> x_list{};
  std::vector<double> y_list{};

  // We set all the values before the minimum to have the same value (blue fix)
  double min_step = (*min_lambda)(z);
  double min_t    = (*min_taueff)(z);
  x_list.push_back(std::numeric_limits<double>::lowest());
  y_list.push_back(min_t);
  x_list.push_back(min_step);
  y_list.push_back(min_t);

  double      max_step  = lyman_lambda[0] * (1 + z);
  double      step      = (max_step - min_step) / 1000.;
  std::size_t next_line = lyman_lambda.size() - 1;
  for (double x = min_step + step; x < max_step; x += step) {
    if (x >= lyman_lambda[next_line] * (1 + z)) {
      x = lyman_lambda[next_line] * (1 + z);
      --next_line;
    }
    x_list.push_back(x);

    double t = t_lyman_series_laf(z, x);
    t += t_lyman_series_dla(z, x);
    t += t_lyman_cont_laf(z, x);
    t += t_lyman_cont_dla(z, x);
    y_list.push_back(std::exp(-t));
  }

  // We set all the values after the first Lyman emission to one
  x_list.push_back(max_step);
  y_list.push_back(1.);
  x_list.push_back(std::numeric_limits<double>::max());
  y_list.push_back(1.);

  return MathUtils::interpolate(x_list, y_list, MathUtils::InterpolationType::LINEAR);
}

static std::map<double, std::unique_ptr<MathUtils::Function>> igm_transmission_map{};
static std::mutex                                             igm_func_mutex{};

static MathUtils::Function& getIgmTransmissionFunc(double z) {
  std::lock_guard<std::mutex> guard{igm_func_mutex};
  auto&                       func_ptr = igm_transmission_map[z];
  if (func_ptr == nullptr) {
    func_ptr = buildIgmTransmissionFunc(z);
  }
  return *func_ptr;
}

XYDataset::XYDataset InoueIgmFunctor::operator()(const XYDataset::XYDataset& sed, double z) const {
  std::vector<std::pair<double, double>> absorbed_values{};
  MathUtils::Function&                   igm_func = getIgmTransmissionFunc(z);
  for (auto& sed_pair : sed) {
    absorbed_values.emplace_back(sed_pair.first, sed_pair.second * igm_func(sed_pair.first));
  }
  return XYDataset::XYDataset{std::move(absorbed_values)};
}

}  // end of namespace PhzModeling
}  // end of namespace Euclid
