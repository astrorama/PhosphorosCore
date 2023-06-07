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
 * @file Sed.cpp
 * @author dubathf
 */

#include "PhzDataModel/Sed.h"


namespace Euclid {
namespace PhzDataModel {

	Sed::Sed(std::vector<std::pair<double, double>> values, double scaling, double diff_scaling) :
		XYDataset::XYDataset(std::move(values)), m_scaling{scaling}, m_diff_scaling{diff_scaling}{}


	Sed::Sed(const Euclid::XYDataset::XYDataset& other, double scaling, double diff_scaling) :
		 XYDataset::XYDataset(other),m_scaling{scaling}, m_diff_scaling{diff_scaling}{}

	Sed Sed::factory(std::vector<std::pair<double, double>> vector_pair, double scaling, double diff_scaling){
		auto xy = XYDataset::factory(vector_pair);
		return Sed(xy,scaling, diff_scaling);
	}

	Sed Sed::factory(const std::vector<double>& x, const std::vector<double>& y, double scaling, double diff_scaling){
		auto xy = XYDataset::factory(x, y);
		return Sed(xy,scaling, diff_scaling);
	}

	void Sed::setScaling(double new_scaling){
		m_scaling=new_scaling;
	}

	const double& Sed::getScaling() const{
		return m_scaling;
	}

	void Sed::setDiffScaling(double new_diff_scaling){
		m_diff_scaling=new_diff_scaling;
	}

	const double& Sed::getDiffScaling() const{
		return m_diff_scaling;
	}
}
}
