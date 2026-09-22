#include <vector>
#include <memory>
#include <string>
#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/FileSystemProvider.h"


namespace Euclid {
namespace XYDatasetSet {
// Mock class definition
class MockXYDatasetProvider : public XYDataset::XYDatasetProvider {
public:
  MockXYDatasetProvider(std::string base_folder, double starting_value, std::string param_value){
      m_base_folder = base_folder;
      m_starting_value=starting_value;
      m_param_value=param_value;
      m_names.push_back({base_folder+"/filter_1"});
      m_names.push_back({base_folder+"/filter_2"});
      m_names.push_back({base_folder+"/filter_3"});
      m_names.push_back({base_folder+"/sub_folder/filter_3"});
  }
   
  std::unique_ptr<XYDataset::XYDataset> getDataset(const XYDataset::QualifiedName& qualified_name) override {
      std::vector<std::pair<double, double>> values;
      values.push_back(std::make_pair(1.0,m_starting_value));
      values.push_back(std::make_pair(2.0,m_starting_value+1.0));
      values.push_back(std::make_pair(3.0,m_starting_value+2.0));
      values.push_back(std::make_pair(4.0,m_starting_value+3.0));
      values.push_back(std::make_pair(5.0,m_starting_value+4.0));
      return std::make_unique<XYDataset::XYDataset>(values);
  }

  std::vector<XYDataset::QualifiedName> listContents(const std::string& group) override {
      if (group=="" || group==m_base_folder) {
          return m_names;
      } else if (group==m_base_folder+"/sub_folder") {
         std::vector<XYDataset::QualifiedName> sub_names;
         sub_names.push_back({m_base_folder+"/sub_folder/filter_3"});
         return  sub_names;
      } else {
        return {};
      } 
  }

  std::string getParameter(const XYDataset::QualifiedName& qualified_name, const std::string& key_word) override{
      return key_word+"_"+m_param_value;
  }
  
 private:
   std::string m_base_folder="empty";
   double m_starting_value=0;
   std::string m_param_value="empty";
   std::vector<XYDataset::QualifiedName> m_names;
};

}
}

