/*
 * Copyright (C) 2012-2026 Euclid Science Ground Segment
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
 * @file src/lib/FileSetProvider.cpp
 *
 * @date 2026-09-22
 * @author Dubathf
 */

#include <numeric>
#include <filesystem>
#include "XYDatasetSet/FileSetProvider.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include <CCfits/CCfits>
#include "Table/FitsReader.h"


namespace Euclid {
namespace XYDatasetSet {

 static Elements::Logging logger = Elements::Logging::getLogger("FileSetProvider");
 
 
 static std::string checkEndSlashes(const std::string& input_str) {

  std::string output_str{};

  size_t pos = input_str.find_last_not_of("/");
  if (pos != input_str.length() - 1) {
    // add one
    output_str = input_str.substr(0, pos + 1) + "/";
  } else {
    // No slash at the end
    output_str = input_str + "/";
  }

  return (output_str);
}
 
static std::vector<std::filesystem::path> getRecursiveDirectoryContents(const std::filesystem::path& dir) {
  std::vector<std::filesystem::path> result{};
  for (std::filesystem::directory_iterator iter{dir}; iter != std::filesystem::directory_iterator{}; ++iter) {
   if (std::filesystem::is_directory(*iter)) {
      auto sub_dir_contents = getRecursiveDirectoryContents(*iter);
      std::copy( sub_dir_contents.begin(), sub_dir_contents.end(), std::back_inserter(result));
    } else {
      result.push_back(*iter);
    }
  }
  return result;
}
 
 static bool isDatasetSetFIle(const std::filesystem::path& path) {
    if (std::filesystem::is_regular_file(path) && path.extension()==".fits") {
      bool is_a_dataset_file = true;
      try {
          auto fits = make_unique<CCfits::FITS>(path, CCfits::RWmode::Read);
          const CCfits::ExtHDU& sample_table_hdu = fits->extension(1);
          is_a_dataset_file &= sample_table_hdu.name()=="SAMPLING";
          const CCfits::ExtHDU& values_table_hdu = fits->extension(2);
          is_a_dataset_file &= values_table_hdu.name()=="VALUES";
          const CCfits::ExtHDU& param_table_hdu = fits->extension(3);
          is_a_dataset_file &= param_table_hdu.name()=="PARAMETERS";
      } catch (const CCfits::FitsException&) {
          is_a_dataset_file = false;
      }
      return is_a_dataset_file;
    } else {
        return false;
    }
 }
 
 
 static std::vector<std::string> getDataSetNames(const std::filesystem::path& path) {
    auto fits = make_unique<CCfits::FITS>(path, CCfits::RWmode::Read);
    const CCfits::ExtHDU& values_table_hdu = fits->extension(2);
    auto table = Table::FitsReader{values_table_hdu}.read();
    std::vector<std::string> name_vector(table.size());
    std::transform(table.begin(), table.end(), name_vector.begin(), [](const Table::Row& row) {
        return boost::get<std::string>(row[0]);
    });
    return name_vector;
 }
 
 static std::vector<double> getSampling(const std::filesystem::path& path) {
    auto fits = make_unique<CCfits::FITS>(path, CCfits::RWmode::Read);
    const CCfits::ExtHDU& sampling_table_hdu = fits->extension(1);
    auto table = Table::FitsReader{sampling_table_hdu}.read();
    std::vector<double> sampling_vector(table.size());
    std::transform(table.begin(), table.end(), sampling_vector.begin(), [](const Table::Row& row) {
        return  boost::get<double>(row[0]);
    });
    return sampling_vector;
 }
 
 static std::vector<double> getValues(const std::filesystem::path& path, const std::string& name) {
    auto fits = make_unique<CCfits::FITS>(path, CCfits::RWmode::Read);
    const CCfits::ExtHDU& values_table_hdu = fits->extension(2);
    auto table = Table::FitsReader{values_table_hdu}.read();
    
    for (auto row_iter = table.begin(); row_iter!= table.end(); ++row_iter) {
        if ( boost::get<std::string>((*row_iter)[0]) == name) {
            return  boost::get<std::vector<double>>((*row_iter)[1]);
        }
    }
    
    throw Elements::Exception() << "The DataSet " << name <<  " is not present in file "<< path.string();
 }
 
 static std::string getparam(const std::filesystem::path& path,const std::string& name, const std::string& param) {
    auto fits = make_unique<CCfits::FITS>(path, CCfits::RWmode::Read);
    const CCfits::ExtHDU& values_table_hdu = fits->extension(3);
    auto table = Table::FitsReader{values_table_hdu}.read();
    for (auto row_iter = table.begin(); row_iter!= table.end(); ++row_iter) {
        if ( boost::get<std::string>((*row_iter)[0]) == name) {
          if ( boost::get<std::string>((*row_iter)[1]) == param) {
             return  boost::get<std::string>((*row_iter)[2]);
          }
        }
    }
    
    return "";
 }
 
 FileSetProvider::FileSetProvider(const std::string& root_path) : XYDatasetProvider(), m_root_path(root_path) {
  
  // Make sure the root path finishes with a "/" and only one
  m_root_path = checkEndSlashes(m_root_path);

  // Convert path to boost filesytem object
  std::filesystem::path fspath(m_root_path);
  if (!std::filesystem::exists(fspath)) {
    throw Elements::Exception() << "From FileSetProvider: root path not found : " << fspath;
  }

  // Get all files below the root directory
  if (std::filesystem::is_directory(fspath)) {
    auto dir_contents = getRecursiveDirectoryContents(fspath);
    for (const auto& file : dir_contents) {
      if (isDatasetSetFIle(file)) {
        logger.debug("Found valid file " + file.string());
        auto groups_str = file.parent_path().string() +"/"+ file.stem().string();
        // Remove the root part
        groups_str = groups_str.substr(m_root_path.length(), groups_str.length());
        // Split by the character '/'
        std::vector<std::string> groups{};
        boost::split(groups, groups_str, boost::is_any_of("/"));
        
        auto names = getDataSetNames(file);
        for(auto& name : names) {
            XYDataset::QualifiedName qualified_name{groups, name};
            m_order_names.push_back(qualified_name);
        }
      }
    }
  } 
}

 std::vector<XYDataset::QualifiedName>  FileSetProvider::listContents(const std::string& group) {
    if (group=="") {
        return m_order_names;
    } else {
        std::vector<XYDataset::QualifiedName> return_names;
        for (auto& name : m_order_names) {
            if (name.qualifiedName().rfind(group, 0) == 0) {
                return_names.push_back(name);
            }
        } 
        return return_names; 
    }
 }

 std::unique_ptr<XYDataset::XYDataset>  FileSetProvider::getDataset(const XYDataset::QualifiedName& qualified_name) {
   auto dataset_name = qualified_name.datasetName();
   auto file_path_str = m_root_path + "/" + qualified_name.qualifiedName();
   std::filesystem::path file_path(file_path_str);
   file_path_str = file_path.parent_path().string() + ".fits";
   std::filesystem::path fspath(file_path_str);
   
   logger.debug()<< "getting getDataset for " << file_path_str << " " << dataset_name;
   auto sampling = getSampling(fspath);
   auto values = getValues(fspath, dataset_name);
   auto dataset = XYDataset::XYDataset::factory(sampling, values);
   return std::unique_ptr<XYDataset::XYDataset>(new XYDataset::XYDataset{dataset});
 }

 std::string  FileSetProvider::getParameter(const XYDataset::QualifiedName& qualified_name, const std::string& key_word) {
   auto dataset_name = qualified_name.datasetName();
   auto file_path_str = m_root_path + "/" + qualified_name.qualifiedName();
   std::filesystem::path file_path(file_path_str);
   file_path_str = file_path.parent_path().string() + ".fits";
   std::filesystem::path fspath(file_path_str);
   return getparam(fspath, dataset_name, key_word);
 }

} /* namespace XYDatasetSet */
}  // end of namespace Euclid
