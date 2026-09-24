/*
 * Copyright (C) 2012-2021 Euclid Science Ground Segment
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
 * @file XYDatasetSet/FileSystemSetProvider.h
 *
 * @date 2026-09-22
 * @author Dubathf
 */

#ifndef FILESETPROVIDER_H_
#define FILESETPROVIDER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ElementsKernel/Export.h"

#include "XYDataset/FileParser.h"
#include "XYDataset/XYDataset.h"
#include "XYDataset/XYDatasetProvider.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace XYDatasetSet {

/**
 * @class FileSetProvider
 *
 * @brief
 * The FileSetProvider allows to read multiple XYDatasets from a container fits file
 *
 */

class FileSetProvider : public XYDataset::XYDatasetProvider {
public:
  
  FileSetProvider(const std::string& root_path);

  std::unique_ptr<XYDataset::XYDataset> getDataset(const XYDataset::QualifiedName& qualified_name) override;

  std::vector<XYDataset::QualifiedName> listContents(const std::string& group) override;

  std::string getParameter(const XYDataset::QualifiedName& qualified_name, const std::string& key_word) override;
  
  // Default destructor
  ~FileSetProvider() = default;

private:
  std::string                           m_root_path;
  std::vector<XYDataset::QualifiedName> m_order_names;
};

} /* namespace XYDatasetSet */
}  // end of namespace Euclid

#endif  // FILESETPROVIDER_H_
