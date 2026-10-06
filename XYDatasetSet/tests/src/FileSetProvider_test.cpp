#include <boost/test/unit_test.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>
#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/FileSystemProvider.h"

#include "XYDatasetSet/FileSetProvider.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Auxiliary.h"


BOOST_AUTO_TEST_SUITE(FileSetProvider_test)

BOOST_AUTO_TEST_CASE(constructor_test) {
     Elements::TempDir top_dir{"Temporary_test-%%%%%%%"};
     auto extractPath = (top_dir.path() / "testing").string();
     auto zip_test_data = Elements::getAuxiliaryPath("XYDatasetSet/test_data.zip");
     std::string command = "unzip " + zip_test_data.string() + " -d " + extractPath;
     int result = system(command.c_str());
     
     BOOST_CHECK(result == 0);
     
     Euclid::XYDatasetSet::FileSetProvider provider{extractPath};
}

BOOST_AUTO_TEST_CASE(listContents_test) {
     Elements::TempDir top_dir{"Temporary_test-%%%%%%%"};
     auto extractPath = (top_dir.path() / "testing").string();
     auto zip_test_data = Elements::getAuxiliaryPath("XYDatasetSet/test_data.zip");
     std::string command = "unzip " + zip_test_data.string() + " -d " + extractPath;
     system(command.c_str());
     Euclid::XYDatasetSet::FileSetProvider provider{extractPath};
     
     auto listing = provider.listContents("");
     BOOST_CHECK_EQUAL(listing.size(), 6);
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f1")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f2")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f3")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f4")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f5")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f6")) != listing.end());
     
     listing = provider.listContents("base_folder");
     BOOST_CHECK_EQUAL(listing.size(), 6);
     
    listing = provider.listContents("base_folder/file_1");
     BOOST_CHECK_EQUAL(listing.size(), 3);
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f1")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f2")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/file_1/f3")) != listing.end());
     
     listing = provider.listContents("base_folder/sub_folder");
     BOOST_CHECK_EQUAL(listing.size(), 3);
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f4")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f5")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f6")) != listing.end());
     
     listing = provider.listContents("base_folder/sub_folder/file_2");
     BOOST_CHECK_EQUAL(listing.size(), 3);
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f4")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f5")) != listing.end());
     BOOST_CHECK(std::find(listing.begin(),listing.end(), Euclid::XYDataset::QualifiedName("base_folder/sub_folder/file_2/f6")) != listing.end());
     
}


BOOST_AUTO_TEST_CASE(getDataset_test) {
     Elements::TempDir top_dir{"Temporary_test-%%%%%%%"};
     auto extractPath = (top_dir.path() / "testing").string();
     auto zip_test_data = Elements::getAuxiliaryPath("XYDatasetSet/test_data.zip");
     std::string command = "unzip " + zip_test_data.string() + " -d " + extractPath;
     system(command.c_str());
     Euclid::XYDatasetSet::FileSetProvider provider{extractPath};
     
     auto dataset_ptr = provider.getDataset({"base_folder/file_1/f2"});
     
     std::vector<double> expected_sampling {1,2,3,4,5,6,7,8,9,10};
     std::vector<double> expected_values {0,0,0,0,0,1,1,0,0,0};
     
     BOOST_CHECK_EQUAL(10, dataset_ptr->size());
     
     auto dataset_iter = dataset_ptr->begin();
     for (size_t idx = 0; idx < 10; ++idx){
        BOOST_CHECK_CLOSE((*dataset_iter).first +1.0, expected_sampling[idx] +1.0, 1e-3);   // 1 added to avoid check_close wrt 0
        BOOST_CHECK_CLOSE((*dataset_iter).second +1.0, expected_values[idx] +1.0, 1e-3);  // 1 added to avoid check_close wrt 0
        ++dataset_iter;
     }
}

BOOST_AUTO_TEST_CASE(getParameter_test) {
     Elements::TempDir top_dir{"Temporary_test-%%%%%%%"};
     auto extractPath = (top_dir.path() / "testing").string();
     auto zip_test_data = Elements::getAuxiliaryPath("XYDatasetSet/test_data.zip");
     std::string command = "unzip " + zip_test_data.string() + " -d " + extractPath;
     system(command.c_str());
     Euclid::XYDatasetSet::FileSetProvider provider{extractPath};
     
     auto param = provider.getParameter({"base_folder/file_1/f2"},"PARAMETER");
     auto comment = provider.getParameter({"base_folder/file_1/f2"},"COMMENT");
     auto other = provider.getParameter({"base_folder/file_1/f2"},"OTHER");
     
     BOOST_CHECK_EQUAL(param, "METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]");
     BOOST_CHECK_EQUAL(comment, "WAVELENGTH UNIT=Angstrom");
     BOOST_CHECK_EQUAL(other, "");
}


BOOST_AUTO_TEST_SUITE_END()




