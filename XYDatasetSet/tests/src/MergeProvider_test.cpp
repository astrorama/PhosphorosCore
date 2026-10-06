#include <boost/test/unit_test.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/CachedProvider.h"

#include "XYDatasetSet/MergeProvider.h"
#include "MockXYDatasetProvider.h"


BOOST_AUTO_TEST_SUITE(MergeProvider_test)

BOOST_AUTO_TEST_CASE(constructor_test) {

    
    auto provider1 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_1",0.0,"toto");
    auto provider2 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_2",100.0,"titi");
    
    std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> providers;
    providers.push_back(std::move(provider1));
    providers.push_back(std::move(provider2));

    Euclid::XYDatasetSet::MergeProvider merge_provider(std::move(providers));
}


BOOST_AUTO_TEST_CASE(listContents_test) {
    auto provider1 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_1",0.0,"toto");
    auto provider2 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_2",100.0,"titi");
    
    std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> providers;
    providers.push_back(std::move(provider1));
    providers.push_back(std::move(provider2));

    Euclid::XYDatasetSet::MergeProvider merge_provider(std::move(providers));

    auto content = merge_provider.listContents("");
    BOOST_CHECK_EQUAL(8, content.size());
    std::vector<std::string> expected{"base_1/filter_1","base_1/filter_2","base_1/filter_3","base_1/sub_folder/filter_3","base_2/filter_1","base_2/filter_2","base_2/filter_3","base_2/sub_folder/filter_3"};
    for (auto& exp_name : expected) {
        BOOST_CHECK(std::find(content.begin(), content.end(), exp_name)!=content.end());
    }
    
    content = merge_provider.listContents("base_1");
    BOOST_CHECK_EQUAL(4, content.size());
    expected = std::vector<std::string>{"base_1/filter_1","base_1/filter_2","base_1/filter_3","base_1/sub_folder/filter_3"};
    for (auto& exp_name : expected) {
        BOOST_CHECK(std::find(content.begin(), content.end(), exp_name)!=content.end());
    }
        
    content = merge_provider.listContents("base_2");
    BOOST_CHECK_EQUAL(4, content.size());
    expected = std::vector<std::string>{"base_2/filter_1","base_2/filter_2","base_2/filter_3","base_2/sub_folder/filter_3"};
    for (auto& exp_name : expected) {
        BOOST_CHECK(std::find(content.begin(), content.end(), exp_name)!=content.end());
    }
    
    content = merge_provider.listContents("base_2/sub_folder");
    BOOST_CHECK_EQUAL(1, content.size());
    expected = std::vector<std::string>{"base_2/sub_folder/filter_3"};
    for (auto& exp_name : expected) {
        BOOST_CHECK(std::find(content.begin(), content.end(), exp_name)!=content.end());
    }
    
    content = merge_provider.listContents("non_existing");
    BOOST_CHECK_EQUAL(0, content.size());
    
}

BOOST_AUTO_TEST_CASE(getDataset_test) {
    auto provider1 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_1",0.0,"toto");
    auto provider2 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_2",100.0,"titi");
    
    std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> providers;
    providers.push_back(std::move(provider1));
    providers.push_back(std::move(provider2));

    Euclid::XYDatasetSet::MergeProvider merge_provider(std::move(providers));
    
    auto data_set_1 = merge_provider.getDataset({"base_1/sub_folder/filter_3"});
    auto it = data_set_1->begin();
    BOOST_CHECK(1.0 == it->first);
    BOOST_CHECK(0.0 == it->second);
    ++it;
    BOOST_CHECK(2.0 == it->first);
    BOOST_CHECK(1.0 == it->second);
    
    auto data_set_2 = merge_provider.getDataset({"base_2/filter_2"});
    it = data_set_2->begin();
    BOOST_CHECK(1.0 == it->first);
    BOOST_CHECK(100.0 == it->second);
    ++it;
    BOOST_CHECK(2.0 == it->first);
    BOOST_CHECK(101.0 == it->second);
}

BOOST_AUTO_TEST_CASE(getParameter_test) {
    auto provider1 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_1",0.0,"toto");
    auto provider2 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_2",100.0,"titi");
    
    std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> providers;
    providers.push_back(std::move(provider1));
    providers.push_back(std::move(provider2));

    Euclid::XYDatasetSet::MergeProvider merge_provider(std::move(providers));
    
    auto parameter_1 = merge_provider.getParameter({"base_1/sub_folder/filter_3"}, "Key1");
    BOOST_CHECK_EQUAL("Key1_toto", parameter_1);
    
    auto parameter_2 = merge_provider.getParameter({"base_2/filter_2"}, "Key2");
    BOOST_CHECK_EQUAL("Key2_titi", parameter_2); 
}


BOOST_AUTO_TEST_CASE(getDataset_cached_test) {
    auto provider1 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_1",0.0,"toto");
    auto provider2 = std::make_unique<Euclid::XYDatasetSet::MockXYDatasetProvider>("base_2",100.0,"titi");
    
    std::vector<std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>> providers;
    providers.push_back(std::move(provider1));
    providers.push_back(std::move(provider2));

    auto     merge_provider = std::make_unique<Euclid::XYDatasetSet::MergeProvider>(std::move(providers));
    
    std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> c_provider(std::move(merge_provider));
    
    auto data_set_1 = c_provider->getDataset({"base_1/sub_folder/filter_3"});
    auto it = data_set_1->begin();
    BOOST_CHECK(1.0 == it->first);
    BOOST_CHECK(0.0 == it->second);
    ++it;
    BOOST_CHECK(2.0 == it->first);
    BOOST_CHECK(1.0 == it->second);
    
    auto data_set_2 = c_provider->getDataset({"base_2/filter_2"});
    it = data_set_2->begin();
    BOOST_CHECK(1.0 == it->first);
    BOOST_CHECK(100.0 == it->second);
    ++it;
    BOOST_CHECK(2.0 == it->first);
    BOOST_CHECK(101.0 == it->second);
}



BOOST_AUTO_TEST_SUITE_END()




