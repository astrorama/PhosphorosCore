/** 
 * @file LikelihoodHandler.cpp
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <CCfits/CCfits>
#include "Table/Table.h"
#include "Table/FitsWriter.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "PhzUtils/FileUtils.h"

namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzOutput {

static Elements::Logging logger = Elements::Logging::getLogger("PhzOutput");

LikelihoodHandler::LikelihoodHandler(boost::filesystem::path out_dir)
          : m_out_dir{std::move(out_dir)} {

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryOnly(m_out_dir.string());

}
          
template <typename AxisType>
std::string convertAxisValueToString(const AxisType& value) {
  return std::to_string(value);
}

template <>
std::string convertAxisValueToString(const XYDataset::QualifiedName& value) {
  return value.qualifiedName();
}

template <int I>
void addAxis(const string& filename, const PhzDataModel::LikelihoodGrid& grid) {
  auto& axis = grid.getAxis<I>();
  
  std::vector<Table::ColumnInfo::info_type> info_list {
    Table::ColumnInfo::info_type {"Index", typeid(int32_t)},
    Table::ColumnInfo::info_type {"Value", typeid(std::string)}
  };
  std::shared_ptr<Table::ColumnInfo> column_info {new Table::ColumnInfo {std::move(info_list)}};
  
  std::vector<Table::Row> row_list {};
  for (size_t i=0; i<axis.size(); ++i) {
    row_list.push_back(Table::Row{{(int)i, convertAxisValueToString(axis[i])}, column_info});
  }
  Table::Table table {row_list};
  
  CCfits::FITS fits {filename, CCfits::RWmode::Write};
  Table::FitsWriter writer {Table::FitsWriter::Format::BINARY};
  writer.write(fits, axis.name(), table);
}

void LikelihoodHandler::handleSourceOutput(const SourceCatalog::Source& source,
                                           const result_type& results) {
  std::string id = std::to_string(source.getId());
  std::string filename = (m_out_dir/(id+".fits")).string();
  auto& likelihood_grid = std::get<2>(results).at("");
  
  // Create the first HDU with the array. We do that in a scope so the file is
  // created and the data are flushed into it before we continue.
  {
    long naxis = likelihood_grid.axisNumber();
    auto& axes = likelihood_grid.getAxesTuple();
    long naxes[4] = {(long)std::get<0>(axes).size(),
                     (long)std::get<1>(axes).size(),
                     (long)std::get<2>(axes).size(),
                     (long)std::get<3>(axes).size()};
    CCfits::FITS fits("!"+filename, DOUBLE_IMG, naxis, naxes);
    fits.pHDU().addKey("ID", source.getId(), "");
    std::valarray<double> data (likelihood_grid.size());
    int i = 0;
    for (auto value : likelihood_grid) {
      data[i] = value;
      ++i;
    }
    fits.pHDU().write(1, likelihood_grid.size(), data);
    logger.info() << "Created likelihood grid for source " << id << " in file " << filename;
  }
  
  // Now add a binary table for each axis with its values
  addAxis<0>(filename, likelihood_grid);
  addAxis<1>(filename, likelihood_grid);
  addAxis<2>(filename, likelihood_grid);
  addAxis<3>(filename, likelihood_grid);
}

} // end of namespace PhzOutput
} // end of namespace Euclid
