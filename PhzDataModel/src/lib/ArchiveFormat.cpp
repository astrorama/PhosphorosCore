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

/**
 * @file PhzDataModel/ArchiveFormat.cpp
 * @date Nov 26, 2019
 * @author Alejandro Alvarez Ayllon
 */

#include "PhzDataModel/ArchiveFormat.h"
#include <cstring>
#include <fstream>
#include <iostream>

namespace Euclid {
namespace PhzDataModel {

ArchiveFormat archiveFormatFromString(const std::string& str) {
  if (str == "TEXT")
    return ArchiveFormat::TEXT;
  else if (str == "BINARY")
    return ArchiveFormat::BINARY;
  return ArchiveFormat::UNKNOWN;
}

ArchiveFormat guessArchiveFormat(std::ifstream& in) {
  ArchiveFormat format = ArchiveFormat::BINARY;

  auto pos        = in.tellg();
  char buffer[64] = {0};
  in.read(buffer, sizeof(buffer));

  if (strncmp(buffer, "22 serialization::archive", 25) == 0) {
    format = ArchiveFormat::TEXT;
  }

  in.seekg(pos);
  return format;
}

std::ostream& operator<<(std::ostream& out, const ArchiveFormat& fmt) {
  switch (fmt) {
  case ArchiveFormat::TEXT:
    out << "TEXT";
    break;
  case ArchiveFormat::BINARY:
    out << "BINARY";
    break;
  default:
    out << "UNKNOWN";
  }
  return out;
}

}  // end namespace PhzDataModel
}  // end namespace Euclid
