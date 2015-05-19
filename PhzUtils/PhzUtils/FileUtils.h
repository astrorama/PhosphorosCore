/*
 * FileUtils.h
 *
 *  Created on: May 7, 2015
 *      Author: morisset
 */

#ifndef PHZUTILS_PHZUTILS_FILEUTILS_H_
#define PHZUTILS_PHZUTILS_FILEUTILS_H_

namespace Euclid {
namespace PhzUtils {

/// Check if the directory exists otherwise it creates it. It also
/// makes sure the given path is really a directory.
/// param: directory_name, only the path of a directory
ELEMENTS_API void createDirectoryIfAny(const std::string& directory_name);

/// Check if a file can be created at the given location
/// param : directory_name_filename, filename with the path or just a path
/// param : path_only, true if directory_name_filename is a path only
ELEMENTS_API void checkWritePermission(const std::string& directory_name_filename, bool path_only = false) ;

/// This function is used when we need to check for a path ONLY
/// Check if the directory exists and we are able to write a file there
/// This function calls the createDirectoryIfAny and checkWritePermission functions
/// param: directory_path which is a directory path ONLY
ELEMENTS_API void checkCreateDirectoryOnly(const std::string& directory_path);

/// This function is used when we need to check for a path and a filename
/// Check the directory exists and we are able to write a file there
/// This function calls the createDirectoryIfAny and checkWritePermission functions
/// param: path_and_filename, filename and path
ELEMENTS_API void checkCreateDirectoryWithFile(const std::string& path_and_filename);

} /* namespace FileUtils */
} // end of namespace Euclid



#endif /* PHZUTILS_PHZUTILS_FILEUTILS_H_ */
