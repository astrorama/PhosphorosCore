/**
 * @file PhzTest/ModelFixingAttributeFromRow.h
 *
 * Created on: 2016-02-08
 *     Author: Florian Dubath
 */
#ifndef MODELFIXINGATTRIBUTEFROMROW_H_
#define MODELFIXINGATTRIBUTEFROMROW_H_

#include <string>
#include <typeindex>
#include "SourceCatalog/Attribute.h"
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/Catalog.h"
#include "Table/Table.h"
#include "XYDataset/QualifiedName.h"

#include "PhzTest/ModelFixingAttribute.h"

namespace Euclid {
namespace PhzTest {

/**
 * @class Coordinates
 * @brief Get the SED (SED) Redshift (Z) ReddeningCurve (RED_CURVE) and
 * E(B-V) (EBV) of the model fitting the source out of the catalog row
 */
class ModelFixingAttributeFromRow : public SourceCatalog::AttributeFromRow {
public:
  ModelFixingAttributeFromRow(std::shared_ptr<Euclid::Table::ColumnInfo> column_info_ptr){
    // SED
    auto sed_column = column_info_ptr->find("SED");
    if (sed_column == nullptr) {
      throw Elements::Exception() << "The column 'SED' is missing.";
    }

    // Z
    auto z_column = column_info_ptr->find("Z");
    if( z_column == nullptr || std::type_index(typeid(double)) != column_info_ptr->getDescription(*z_column).type ){
      throw Elements::Exception() << "The column 'Z' is missing or do not have 'double' type.";
    }

    // Red curve
    auto red_curve_column = column_info_ptr->find("RED_CURVE");
    if (red_curve_column == nullptr) {
      throw Elements::Exception() << "The column 'RED_CURVE' is missing.";
    }

    // EBV
    auto ebv_column = column_info_ptr->find("EBV");
    if( ebv_column == nullptr || std::type_index(typeid(double)) != column_info_ptr->getDescription(*(ebv_column)).type ){
      throw Elements::Exception() << "The column 'EBV' is missing or do not have 'double' type.";
    }

    m_column_index.push_back(*(sed_column));
    m_column_index.push_back(*(z_column));
    m_column_index.push_back(*(red_curve_column));
    m_column_index.push_back(*(ebv_column));

  };

virtual ~ModelFixingAttributeFromRow(){}

/**
 * @brief Create a ModelFixingAttribute from a Table row
 * @details Create a photometricAttribute from a Table row using the mapping included in this object
 * @param row A Table row
 * @return A unique pointer to a (Photometry) Attribute
 */
std::unique_ptr<SourceCatalog::Attribute> createAttribute(const Euclid::Table::Row& row) override {

  Euclid::Table::Row::cell_type sed_cell = row[m_column_index[0]];
  Euclid::Table::Row::cell_type z_cell = row[m_column_index[1]];
  Euclid::Table::Row::cell_type red_cell = row[m_column_index[2]];
  Euclid::Table::Row::cell_type ebv_cell = row[m_column_index[3]];

  XYDataset::QualifiedName sed=XYDataset::QualifiedName{boost::get<std::string>(sed_cell)};
  double z = boost::get<double>(z_cell);
  XYDataset::QualifiedName red=XYDataset::QualifiedName{boost::get<std::string>(red_cell)};
  double ebv = boost::get<double>(ebv_cell);

  return std::unique_ptr<SourceCatalog::Attribute>{new ModelFixingAttribute{sed, z, red, ebv}};
}

private:
std::vector<int> m_column_index{};

};

} // namespace PhzTest
} // end of namespace Euclid

#endif // MODELFIXINGATTRIBUTEFROMROW_H_
