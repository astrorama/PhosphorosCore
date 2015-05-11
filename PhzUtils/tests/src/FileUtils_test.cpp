/*
 * FileUtils_test.cpp
 *
 *  Created on: May 7, 2015
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Temporary.h"
#include <boost/test/test_tools.hpp>

#include "ElementsKernel/Exception.h"
#include "PhzUtils/FileUtils.h"

namespace pu = Euclid::PhzUtils;
namespace fs = boost::filesystem;

struct FileUtils_Fixture {

  Elements::TempDir temp_dir {};
  fs::path directory_name = temp_dir.path();

  FileUtils_Fixture() {

  }
  ~FileUtils_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FileUtils_test)

//-----------------------------------------------------------------------------
// Test the exception of the CreateDirectoryIfAny function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CreateDirectoryIfAny_exception_test, FileUtils_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception of the directory_write_protected function");
  BOOST_TEST_MESSAGE(" ");

  fs::path dir = directory_name/"dir_write_protected";

  pu::createDirectoryIfAny(dir.string());
  // Remove protection
  fs::permissions(dir, fs::perms::remove_perms|fs::perms::owner_write|fs::perms::others_write|fs::perms::group_write);

  fs::path dir2 = directory_name/"dir_write_protected/test";

  BOOST_CHECK_THROW(pu::createDirectoryIfAny(dir2.string()), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the exception of CheckWritePermission function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CheckWritePermission_exception_test, FileUtils_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception of the CheckWritePermission function");
  BOOST_TEST_MESSAGE(" ");

  fs::path dir = directory_name/"dir_write_protected";
  fs::path dir_filename = directory_name/"dir_write_protected/test_file.txt";

  pu::createDirectoryIfAny(dir.string());
  // Remove protection
  fs::permissions(dir, fs::perms::remove_perms|fs::perms::owner_write|fs::perms::others_write|fs::perms::group_write);

  fs::path dir2 = directory_name/"dir_write_protected/test";

  BOOST_CHECK_THROW(pu::checkWritePermission(dir_filename.string(), false), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the exception of checkDirectoryWithFile function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(checkCreateDirectoryWithFile_exception_test, FileUtils_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception from the checkDirAndWritePermission function");
  BOOST_TEST_MESSAGE(" ");

  fs::path dir = directory_name/"dir_write_protected";
  fs::path dir_filename = directory_name/"dir_write_protected/test_file.txt";

  pu::createDirectoryIfAny(dir.string());
  fs::permissions(dir, fs::perms::remove_perms|fs::perms::owner_write|fs::perms::others_write|fs::perms::group_write);

  BOOST_CHECK_THROW(pu::checkCreateDirectoryWithFile(dir_filename.string()), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the exception of checkDirectoryWithFile function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(checkCreateDirectoryOnly_exception_test, FileUtils_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception from the checkDirectoryOnly function");
  BOOST_TEST_MESSAGE(" ");

  fs::path dir = directory_name/"dir_write_protected";

  pu::createDirectoryIfAny(dir.string());
  fs::permissions(dir, fs::perms::remove_perms|fs::perms::owner_write|fs::perms::others_write|fs::perms::group_write);

  BOOST_CHECK_THROW(pu::checkCreateDirectoryOnly(dir.string()), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the exception of checkDirectoryWithFile function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(checkCreateDirectoryOnly_exception_notadirectory_test, FileUtils_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception when not a directory from the checkDirectoryOnly function");
  BOOST_TEST_MESSAGE(" ");

  fs::path dir = directory_name/"dir_write_protected";
  pu::createDirectoryIfAny(dir.string());
  // Create a file at this location
  std::string full_filename = dir.string()+"/zzztestzzz.zzz";
  std::ofstream outfile(full_filename);

  BOOST_CHECK_THROW(pu::checkCreateDirectoryOnly(full_filename), Elements::Exception);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()





