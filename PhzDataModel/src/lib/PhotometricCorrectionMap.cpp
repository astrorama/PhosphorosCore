/**
 * @file PhzDataModel/PhotometryGrid.cpp
 * @date Jnn 12, 2015
 * @author Florian Dubath
 */

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "ElementsKernel/Exception.h"
#include "Table/AsciiReader.h"
#include "Table/AsciiWriter.h"

namespace Euclid {
namespace PhzDataModel {

void writePhotometricCorrectionMap(std::ostream& out, const PhotometricCorrectionMap& corrections) {
  std::vector<Euclid::Table::ColumnInfo::info_type> info_list{
      Euclid::Table::ColumnInfo::info_type("Filter", typeid(std::string)),
      Euclid::Table::ColumnInfo::info_type("Correction", typeid(double)),
  };

  std::shared_ptr<Euclid::Table::ColumnInfo> column_info{new Euclid::Table::ColumnInfo{std::move(info_list)}};

  std::vector<Euclid::Table::Row> row_list{};

  for (auto& corr : corrections) {
    std::vector<Euclid::Table::Row::cell_type> correction_data{corr.first.qualifiedName(), corr.second};
    auto                                       new_row = Euclid::Table::Row(std::move(correction_data), column_info);
    row_list.push_back(std::move(new_row));
  }

  auto table_representation = Euclid::Table::Table{std::move(row_list)};

  Table::AsciiWriter{out}.addData(table_representation);
}

PhotometricCorrectionMap readPhotometricCorrectionMap(std::istream& in) {
  auto table       = Table::AsciiReader{in}.read();
  auto column_info = table.getColumnInfo();

  if (column_info->find("Filter") == nullptr || column_info->find("Correction") == nullptr) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: table must contain columns 'Filter' and 'Correction'";
  }

  if (column_info->getDescription(*(column_info->find("Filter"))).type != typeid(std::string)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the column 'Filter' must be of type 'string'";
  }

  if (column_info->getDescription(*(column_info->find("Correction"))).type != typeid(double)) {
    throw Elements::Exception() << "readPhotometricCorrectionMap: the columns 'Correction' must be of type 'double'";
  }

  auto map = PhotometricCorrectionMap{};
  for (auto& row : table) {
    auto filter_name = XYDataset::QualifiedName(boost::get<std::string>(row["Filter"]));
    map.emplace(std::make_pair(std::move(filter_name), boost::get<double>(row["Correction"])));
  }

  return map;
}

}  // namespace PhzDataModel
}  // namespace Euclid
