/**
 * @file PhzLuminosity/LuminosityFunctionSet.h
 * @date 19 August 2015
 * @author Florian Dubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_

#include <cmath>
#include <map>
#include "PhzLuminosity/LuminosityFunctionValidityDomain.h"
#include "MathUtils/function/Function.h"


namespace Euclid {
namespace PhzLuminosity {

/**
 * @class Euclid::PhzLuminosity::LuminosityFunctionSet
 *
 * @brief The set of Luminosity function covering the full parameter space.
 */
class LuminosityFunctionSet {
public:
  /**
   * @brief Constructor
   *
   * @details Check that the domains provided in the luminosityFunctions vectors
   * do not overlap
   *
   * @param luminosityFunctions A vector of pair which key is the Validity domain
   * and the value the luminosity function for this domain.
   */
  LuminosityFunctionSet(std::vector<std::pair<LuminosityFunctionValidityDomain,
                        std::unique_ptr<MathUtils::Function>>> luminosityFunctions);

  /**
   * @brief Search for the validity domain the coordinate sedGroup, redshift
   * belongs to and return the corresponding pair validity domain/luminosity function.
   *
   * @param sedGroup The name of the SED Group the caller want the Luninosity function for.
   *
   * @param z The redshift the caller want the Luninosity function for.
   */
  const std::pair<LuminosityFunctionValidityDomain, std::unique_ptr<MathUtils::Function>>&
  getLuminosityFunction(const std::string& sedGroup, double z) const ;


private:
  std::vector<std::pair<LuminosityFunctionValidityDomain,
                        std::unique_ptr<MathUtils::Function>>> m_luminosity_functions;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYFUNCTIONSET_H_ */
