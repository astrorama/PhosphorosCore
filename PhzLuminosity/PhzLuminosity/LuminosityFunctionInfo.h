/*
 * LuminosityFunctionInfo.h
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONINFO_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONINFO_H_

#include <string>
#include <vector>

namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::LuminosityFunctionInfo
 *
 * @brief Information needed to build a luminosity function.
 */
class LuminosityFunctionInfo{
public:
  double z_min=-1.;
  double z_max=-1.;
  std::vector<std::string> SEDs{};
  std::string datasetName="";
  double phi_star=0.;
  double mag_star=0.;
  double alpha=0.;

};

}
}
#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONINFO_H_ */
