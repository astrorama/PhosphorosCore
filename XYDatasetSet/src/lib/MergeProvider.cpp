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
 * @file src/lib/MergeProvider.cpp
 *
 * @date 2026-09-22
 * @author Dubathf
 */

#include <numeric>
#include "XYDatasetSet/MergeProvider.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace XYDatasetSet {

 static Elements::Logging logger = Elements::Logging::getLogger("MergeProvider");

 MergeProvider::MergeProvider(std::vector<std::unique_ptr<XYDataset::XYDatasetProvider>> provider_list) {
     m_provider_list = std::move(provider_list);
     for (int idx=0; idx<m_provider_list.size(); ++idx) {
         for(auto item : m_provider_list[idx]->listContents("")) {
             m_provider_lookup.insert(std::make_pair(item, idx));
             auto groups = item.groups();
             std::vector<std::string> grps{};
             for (auto grp : groups) {
                 grps.push_back(grp);
                 std::string joinedString = std::accumulate(grps.begin(), grps.end(), std::string(), [this](const std::string& a, const std::string& b) -> std::string {
                                                                                                                                  return a.empty() ? b : a + "/" + b;});
                 m_group_lookup.insert(std::make_pair(joinedString, idx));
             }
         }
     }
 }

 std::unique_ptr<XYDataset::XYDataset>  MergeProvider::getDataset(const XYDataset::QualifiedName& qualified_name) {
   int idx = m_provider_lookup.at(qualified_name);
   return m_provider_list[idx]->getDataset(qualified_name);
 }

 std::vector<XYDataset::QualifiedName>  MergeProvider::listContents(const std::string& group) {
   if (group==""){
      std::vector<XYDataset::QualifiedName> complete{};
      for (size_t l_idx=0; l_idx<m_provider_list.size(); ++l_idx){
          auto partial = m_provider_list[l_idx]->listContents("");
          complete.insert(complete.end(), partial.begin(), partial.end());
      }
      return complete;
   } else {
     int idx = m_group_lookup.at(group);
     return m_provider_list[idx]->listContents(group);
   }
 }

 std::string  MergeProvider::getParameter(const XYDataset::QualifiedName& qualified_name, const std::string& key_word) {
   int idx = m_provider_lookup.at(qualified_name);
   return m_provider_list[idx]->getParameter(qualified_name, key_word);
 }

} /* namespace XYDatasetSet */
}  // end of namespace Euclid
