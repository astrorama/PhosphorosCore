/** 
 * @file BestModelCatalog.cpp
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <CCfits/CCfits>
#include "ElementsKernel/Logging.h"
#include "Table/Table.h"
#include "Table/AsciiWriter.h"
#include "Table/FitsWriter.h"
#include "PhzOutput/BestModelCatalog.h"
#include "PhzUtils/FileUtils.h"

namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzOutput {

static Elements::Logging logger = Elements::Logging::getLogger("PhzOutput");

BestModelCatalog::~BestModelCatalog() {
  Table::Table out_table {std::move(m_row_list)};
  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(m_out_file.string());
  if (m_format == Format::ASCII) {
    std::ofstream out {m_out_file.string()};
    Table::AsciiWriter().write(out, out_table, false);
  } else {
    CCfits::FITS fits {"!"+m_out_file.string(), CCfits::RWmode::Write};
    Table::FitsWriter().write(fits, "Best Model Catalog", out_table);
  }
  logger.info() << "Created best fit model catalog in file " << m_out_file.string();
}

void BestModelCatalog::handleSourceOutput(const SourceCatalog::Source& source,
                                          const result_type& results) {
  auto& best_model = std::get<0>(results);
  auto sed = best_model.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName();
  int64_t sed_index = best_model.axisIndex<PhzDataModel::ModelParameter::SED>() + 1;
  auto reddening_curve = best_model.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName();
  auto ebv = best_model.axisValue<PhzDataModel::ModelParameter::EBV>();
  auto z = best_model.axisValue<PhzDataModel::ModelParameter::Z>();
  auto scale = std::get<4>(results);
  auto likelihood = std::get<5>(results);
  m_row_list.push_back(Table::Row{{source.getId(), sed, sed_index, reddening_curve, ebv, z, scale, likelihood}, m_column_info});
}

} // end of namespace PhzOutput
} // end of namespace Euclid
