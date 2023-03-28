/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
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

/*
 * @file PPConfig.cpp
 * @author dubathf
 */

#include <cstdlib>
#include <cmath>
#include "PhzDataModel/PPConfig.h"


namespace Euclid {
namespace PhzDataModel {


	PPConfig::PPConfig(double A, double B,  double C,  double D, std::string unit) :
			m_A{A}, m_B{B},m_C{C}, m_D{D}, m_unit{unit}{}

	double PPConfig::getA() const {
		return m_A;
	}

	double PPConfig::getB() const {
		return m_B;
	}

	double PPConfig::getC() const {
		return m_C;
	}

	double PPConfig::getD() const {
		return m_D;
	}

	const std::string& PPConfig::getUnit() const {
		return m_unit;
	}

	double PPConfig::apply(double L0) const {
		if (abs(m_C)<1e-10) {
			return m_A * L0 + m_B;
		} else {
			return m_A * L0 + m_B + m_C * log10(m_D * L0);
		}
	}
}
}
