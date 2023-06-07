/*
 * Copyright (C) 2012-2023 Euclid Science Ground Segment
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
 * @file Sed.h
 *
 * @date 2023/06/07
 * @author dubathf
 */

#ifndef SED_H_
#define SED_H_


#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzDataModel {

class Sed : public XYDataset::XYDataset {
public:
	/// constructor from values
	Sed(std::vector<std::pair<double, double>> values, double scaling=1, double diff_scaling=1);

	/// Copy from Parent
	Sed(const Euclid::XYDataset::XYDataset&, double scaling=1, double diff_scaling=1);

	/// Copy constructor
	Sed(const Sed&) = default;

	/// Move constructor
	Sed(Sed&&) = default;

	/**
	 * @brief
	 * Factory from a vector of pair of doubles
	 */
	static Sed factory(std::vector<std::pair<double, double>> vector_pair, double scaling=1, double diff_scaling=1);

	/**
	* @brief
	* Factory from two vectors of doubles
	*/
	static Sed factory(const std::vector<double>& x, const std::vector<double>& y, double scaling=1, double diff_scaling=1);

	/**
	* @brief Destructor
	*/
	virtual ~Sed() = default;

	/**
	* @brief
	* Scaling accessor
	*/
	void setScaling(double new_scaling);
	const double& getScaling() const;

	void setDiffScaling(double new_diff_scaling);
	const double& getDiffScaling() const;


private:
  double m_scaling=1;
  double m_diff_scaling=1;
};


}
}
#endif /* SED_H_ */

