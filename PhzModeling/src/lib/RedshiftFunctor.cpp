/**
 * @file PhzModeling/RedshiftFunctor.cpp
 * @date Sep 16, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/RedshiftFunctor.h"
#include "XYDataset/XYDataset.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhysicsUtils/CosmologicalDistances.h"

namespace Euclid {
namespace PhzModeling {

RedshiftFunctor::RedshiftFunctor(PhysicsUtils::CosmologicalParameters cosmology)
      :m_cosmology{std::move(cosmology)} {}


Euclid::XYDataset::XYDataset RedshiftFunctor::operator()(
    const Euclid::XYDataset::XYDataset& sed,
    double z) const {

      auto distances = PhysicsUtils::CosmologicalDistances{};
      double luminosity_distance = distances.luminousDistance(z, m_cosmology);
      double factor = 100.0/(luminosity_distance*luminosity_distance*(1+z));

      std::vector<std::pair<double, double>> redshifted_values {};
      for (auto& sed_pair : sed) {
        redshifted_values.emplace_back(
            std::make_pair(sed_pair.first*(1+z),
            sed_pair.second*factor));
      }
      return  Euclid::XYDataset::XYDataset {std::move(redshifted_values)};
}

} // end of namespace PhzModeling
} // end of namespace Euclid

