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
 * @file PhzDataModel/PPConfig.h
 * @author dubathf
 */

#ifndef _PHZDATAMODEL_PPCONFIG_H
#define _PHZDATAMODEL_PPCONFIG_H

#include "ElementsKernel/Exception.h"
#include <map>
#include <memory>

namespace Euclid {
namespace PhzDataModel {

/**
 * Class storing the PP parameter parameterization as
 * pp(L0) = A*L0 + B + C*LOG(D*L0)
 * Where LOG is the logarithm in base 10 and is not evaluated if C==0
 */
class PPConfig {
public:
	PPConfig(double A=0, double B=0,  double C=0,  double D=0, std::string unit="");
	virtual ~PPConfig() = default;
	double getA() const;
	double getB() const;
	double getC() const;
	double getD() const;

	const std::string& getUnit() const;

	double apply(double L0) const;
private:
	double m_A;
	double m_B;
	double m_C;
	double m_D;
	std::string m_unit;
};
}
}
#endif /* _PHZDATAMODEL_PPCONFIG_H */
