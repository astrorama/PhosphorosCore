/**
 * @file LikelihoodHandler.h
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_LIKELIHOODHANDLER_H
#define	PHZOUTPUT_LIKELIHOODHANDLER_H

#include <boost/filesystem.hpp>
#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

template<PhzDataModel::RegionResultType GridType>
class LikelihoodHandler : public OutputHandler {

public:

  LikelihoodHandler(boost::filesystem::path out_dir, bool do_sample, size_t sample_number = 1000, size_t chunk_size = 10000);

  virtual ~LikelihoodHandler();

  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override;


private:

  boost::filesystem::path m_out_dir;
  bool m_do_sample;
  size_t m_sample_number;
  size_t m_chunk_size;

  size_t                        m_current_file_id = 0;
  boost::filesystem::path       m_current_out_file;  // Filename of the output fits file
  std::shared_ptr<CCfits::FITS> m_current_fits_file;  // Pointer to the FITS file object
  std::shared_ptr<Euclid::Table::ColumnInfo> m_ampling_column_info;
  size_t m_counter;   // Counting the number of sources

  boost::filesystem::path       m_index_file;   // Filename of the output fits file
  std::shared_ptr<CCfits::FITS> m_index_fits_file;  // Pointer to the FITS file object
  std::shared_ptr<Euclid::Table::ColumnInfo> m_index_column_info;
  std::vector<Table::Row> m_index_row_list {};
};

} // end of namespace PhzOutput
} // end of namespace Euclid


#include "PhzOutput/_impl/LikelihoodHandler.icpp"

#endif	/* PHZOUTPUT_LIKELIHOODHANDLER_H */

