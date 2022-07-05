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
 * @file ArchiveFormat.h
 * @author Alejandro Alvarez Ayllon
 */

#ifndef PHZDATAMODEL_ARCHIVEFORMAT_H
#define PHZDATAMODEL_ARCHIVEFORMAT_H

#include <iosfwd>

namespace Euclid {
namespace PhzDataModel {

/**
 * @enum ArchiveFormat
 * @brief
 *      This type helps handling types that can be archived - using boost archive -
 *      in multiple formats.
 */
enum class ArchiveFormat { BINARY, TEXT, UNKNOWN };

/**
 * Convert a string representation to the enum
 */
ArchiveFormat archiveFormatFromString(const std::string& str);

/**
 * Guess the format of a given archive
 */
ArchiveFormat guessArchiveFormat(std::ifstream& in);

/**
 * Write to an output stream a string representation of the enum
 */
std::ostream& operator<<(std::ostream& out, const ArchiveFormat& fmt);

}  // end namespace PhzDataModel
}  // end namespace Euclid

#endif  // PHZDATAMODEL_ARCHIVEFORMAT_H
