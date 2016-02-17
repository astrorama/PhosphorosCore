/** 
 * @file BestModelCatalog.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_BESTMODELCATALOG_H
#define	PHZOUTPUT_BESTMODELCATALOG_H

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include "Table/Row.h"
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

class BestModelCatalog : public OutputHandler {
  
public:
  
  enum class Format {
    ASCII, FITS
  };
  
  BestModelCatalog(fs::path out_file, Format format) 
        : m_out_file{std::move(out_file)}, m_format{format} {}
  
  virtual ~BestModelCatalog();
  
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results);
  
private:
  
  fs::path m_out_file;
  Format m_format;
  std::shared_ptr<Table::ColumnInfo> m_column_info {new Table::ColumnInfo {{
      Table::ColumnInfo::info_type("ID", typeid(int64_t)),
      Table::ColumnInfo::info_type("SED", typeid(std::string)),
      Table::ColumnInfo::info_type("SED-Index", typeid(int64_t)),
      Table::ColumnInfo::info_type("ReddeningCurve", typeid(std::string)),
      Table::ColumnInfo::info_type("E(B-V)", typeid(double)),
      Table::ColumnInfo::info_type("Z", typeid(double)),
      Table::ColumnInfo::info_type("Scale", typeid(double)),
      Table::ColumnInfo::info_type("Posterior-Log", typeid(double)),
      Table::ColumnInfo::info_type("1DPDF-Peak-Z", typeid(double))
    }}};
  std::vector<Table::Row> m_row_list;
  
};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_BESTMODELCATALOG_H */

