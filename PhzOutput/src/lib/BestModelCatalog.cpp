/** 
 * @file BestModelCatalog.cpp
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include "ElementsKernel/Logging.h"
#include "Table/Table.h"
#include "Table/AsciiWriter.h"
#include "PhzOutput/BestModelCatalog.h"

static Elements::Logging logger = Elements::Logging::getLogger("BestModelCatalog");

namespace Euclid {
namespace PhzOutput {

BestModelCatalog::~BestModelCatalog() {
  Table::Table out_table {std::move(m_row_list)};
  {
    std::ofstream out {m_out_file.string()};
    Table::AsciiWriter().write(out, out_table);
  }
}

void BestModelCatalog::handleSourceOutput(const SourceCatalog::Source& source,
                                          PhzDataModel::PhotometryGrid::iterator best_model,
                                          const GridContainer::GridContainer<std::vector<double>, double>&) {
  auto sed = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
  auto reddening_curve = best_model.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName();
  auto ebv = best_model.axisValue<PhzDataModel::ModelParameter::EBV>();
  auto z = best_model.axisValue<PhzDataModel::ModelParameter::Z>();
  m_row_list.push_back(Table::Row{{source.getId(), sed, reddening_curve, ebv, z}, m_column_info});
}

} // end of namespace PhzOutput
} // end of namespace Euclid