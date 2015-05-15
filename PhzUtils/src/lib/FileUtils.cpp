/*
 * FileUtils.cpp
 *
 *  Created on: May 7, 2015
 *      Author: Nicolas Morisset
 */

#include <iostream>
#include <string>
#include <fstream>

#include <boost/filesystem.hpp>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "PhzUtils/FileUtils.h"

namespace fs = boost::filesystem;

static Elements::Logging logger = Elements::Logging::getLogger("PhzUtils");

namespace Euclid {
namespace PhzUtils {

/// Check that the directory(directory_name) exists otherwise it does create it
/// param directory_name: it is the full path of the directory
void createDirectoryIfAny(const std::string& directory_name) {

  // Extract the directory name
  fs::path fs_dir(directory_name);

  if (!directory_name.empty()) {
    // Directory does not exist
    if (!fs::exists(fs_dir)) {
      try {
         fs::create_directories(fs_dir);
      }
      catch(std::exception const&  ex) {
        throw Elements::Exception() << "Can not create the  " << directory_name
                                    << " directory(from the createDirectoryIfAny function)!";
      } //EndOfCatch
    }
    else if (!fs::is_directory(fs_dir))
    {
      // The path is not a directory
      throw Elements::Exception() << "The path :  " << directory_name
                                  << " is not a directory!"
                                  << " (from the createDirectoryIfAny function)!";
    }
  }//Eof dir empty

}

/// Check if we are able to write a file at a given location
/// param directory_name_and_or_filename: it is the location path including or
///                                       not the filename
/// param path_only: true, means only the path is provided
void checkWritePermission(const std::string& directory_name_and_or_filename,
                          bool path_only) {

  // Extract the directory and filename if any
  fs::path dir(directory_name_and_or_filename);
  if (!path_only) {
    fs::path fs_dir_filename(directory_name_and_or_filename);
    dir = fs_dir_filename.parent_path();
  }

  // The purpose here is to make sure we are able to
  // write a binary file at this location. We used for that a dummy file
  std::string filename{ dir.string() + "/zzz_phos_random_file_to_be_deleted_zzz.zzz" };
  if (dir.string().empty()) {
     filename = "zzz_phos_random_file_to_be_deleted_zzz.zzz";
  }
  std::fstream test_fstream;
  test_fstream.open(filename, std::fstream::out | std::fstream::binary);
  if ((test_fstream.rdstate() & std::fstream::failbit) != 0) {
    throw Elements::Exception() <<" IO error, can not write any file there : %s "
                                << directory_name_and_or_filename
                                << " (from the CheckDirWritePermission function) ";
  }
  test_fstream.close();
  // Remove file created
  if (std::remove(filename.c_str())) {
    logger.warn() << "Removing temporary file creation failed: \""
                  << filename << "\" (from CheckDirWritePermission)!";
  }

}

// This function checks if the directory exists and if we are able to write a file
// at this location. We need to have a path+filename here
void checkCreateDirectoryOnly(const std::string& directory_path) {

  fs::path directory_path_only(directory_path);
  createDirectoryIfAny(directory_path_only.string());
  checkWritePermission(directory_path, true);

}

// This function checks if the directory exists and if we are able to write a file
// at this location. We need to have a path+filename here
void checkCreateDirectoryWithFile(const std::string& path_and_filename) {

  fs::path fs_dir_filename(path_and_filename);
  fs::path directory_path_only = fs_dir_filename.parent_path();
  createDirectoryIfAny(directory_path_only.string());
  checkWritePermission(path_and_filename);

}



} /* namespace FileUtils */
} // end of namespace Euclid



