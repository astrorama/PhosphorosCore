/**
 * @file AdjustErrorParamMap.cpp
 * @date December 1, 2020
 * @author Dubath F
 */

#include "PhzDataModel/AdjustErrorParamMap.h"
#include "ElementsKernel/Exception.h"
#include "Table/AsciiReader.h"
#include "Table/AsciiWriter.h"

namespace Euclid {
namespace PhzDataModel {

void writeAdjustErrorParamMap(std::ostream& out, const AdjustErrorParamMap& adjustErrorParam) {
  std::vector<Euclid::Table::ColumnInfo::info_type> info_list{
      Euclid::Table::ColumnInfo::info_type("Filter", typeid(std::string)),
      Euclid::Table::ColumnInfo::info_type("Alpha", typeid(double)),
      Euclid::Table::ColumnInfo::info_type("Beta", typeid(double)),
      Euclid::Table::ColumnInfo::info_type("Gamma", typeid(double))};

  std::shared_ptr<Euclid::Table::ColumnInfo> column_info{new Euclid::Table::ColumnInfo{std::move(info_list)}};

  std::vector<Euclid::Table::Row> row_list{};

  for (auto& param : adjustErrorParam) {
    std::vector<Euclid::Table::Row::cell_type> param_data{param.first.qualifiedName(), std::get<0>(param.second),
                                                          std::get<1>(param.second), std::get<2>(param.second)};
    auto                                       new_row = Euclid::Table::Row(std::move(param_data), column_info);
    row_list.push_back(std::move(new_row));
  }

  auto table_representation = Euclid::Table::Table{std::move(row_list)};

  Table::AsciiWriter{out}.addData(table_representation);
}

AdjustErrorParamMap readAdjustErrorParamMap(std::istream& in) {
  auto table       = Table::AsciiReader{in}.read();
  auto column_info = table.getColumnInfo();

  if (column_info->find("Filter") == nullptr || column_info->find("Alpha") == nullptr ||
      column_info->find("Beta") == nullptr || column_info->find("Gamma") == nullptr) {
    throw Elements::Exception()
        << "readPhotometricCorrectionMap: table must contain columns 'Filter', 'Alpha', 'Beta' and 'Gamma'";
  }

  if (column_info->getDescription(*(column_info->find("Filter"))).type != typeid(std::string)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the column 'Filter' must be of type 'string'";
  }

  if (column_info->getDescription(*(column_info->find("Alpha"))).type != typeid(double)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the columns 'Alpha' must be of type 'double'";
  }

  if (column_info->getDescription(*(column_info->find("Beta"))).type != typeid(double)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the columns 'Beta' must be of type 'double'";
  }

  if (column_info->getDescription(*(column_info->find("Gamma"))).type != typeid(double)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the columns 'Gamma' must be of type 'double'";
  }

  auto map = AdjustErrorParamMap{};
  for (auto& row : table) {
    auto filter_name = XYDataset::QualifiedName(boost::get<std::string>(row["Filter"]));
    map.emplace(std::make_pair(std::move(filter_name),
                               std::make_tuple(boost::get<double>(row["Alpha"]), boost::get<double>(row["Beta"]),
                                               boost::get<double>(row["Gamma"]))));
  }

  return map;
}

}  // namespace PhzDataModel
}  // namespace Euclid
