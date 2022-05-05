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
 * @file PhzModeling/RedshiftFunctor.h
 * @date Sep 15, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_REDSHIFTFUNCTOR_H
#define	PHZMODELING_REDSHIFTFUNCTOR_H

#include "PhysicsUtils/CosmologicalParameters.h"

namespace Euclid {
namespace XYDataset {
  class XYDataset;
}


namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::RedshiftFunctor
 * @brief
 * This functor is in charge of redshifting the SED
 * @details
 * This functor is redshifting a SED to a given value z of the redshift.
 * The shift is applied to all the pair of the SED.
 * For a given z, the wavelength component of the SED is multiplied by
 * the factor(1+z)
 * while the corresponding flux is multiplied by 1/((1+z)(DL/10)²)
 */
class RedshiftFunctor {

public:
  RedshiftFunctor(Euclid::PhysicsUtils::CosmologicalParameters cosmology);

	/**
		* @brief Function Call Operator
		* @details
		* Execute the redshifting of a SED
		*
	    * @param sed
		* A XYDataset representing the SED to be redshifted.
		*
		* @param z
		* The redshift to be applied as a double.
		*
		* @return
		* A XYDataset representing the redshifted SED.
		*/
	Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed,
	    double z) const;

private:
	Euclid::PhysicsUtils::CosmologicalParameters m_cosmology;
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_REDSHIFTFUNCTOR_H */
