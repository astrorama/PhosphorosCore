/**
 * @file LikelihoodHandler.h
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_LIKELIHOODHANDLER_H
#define	PHZOUTPUT_LIKELIHOODHANDLER_H

#include <map>
#include <utility>
#include <boost/filesystem.hpp>
#include "PhzDataModel/RegionResults.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzOutput/GridSampler.h"

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
  bool                          m_current_file_has_comment = false;
  boost::filesystem::path       m_current_out_file;  // Filename of the output fits file
  std::shared_ptr<CCfits::FITS> m_current_fits_file;  // Pointer to the FITS file object
  std::shared_ptr<Euclid::Table::ColumnInfo> m_sampling_column_info;
  size_t m_counter;   // Counting the number of sources

  boost::filesystem::path       m_index_file;   // Filename of the output fits file
  std::shared_ptr<CCfits::FITS> m_index_fits_file;  // Pointer to the FITS file object
  std::shared_ptr<Euclid::Table::ColumnInfo> m_index_column_info;
  std::vector<Table::Row> m_index_row_list {};
  GridSampler<GridType> m_grid_sampler;

  std::string getFileName(int file_id);
  void createAndOpenSampleFile();
  void closeSampleFile();

  void exportFullGrid(const SourceCatalog::Source& source,
      const PhzDataModel::SourceResults& results);

  std::vector<Table::Row> drawSample(
      std::string source_id,
      double total_volume,
      const std::map<size_t, double>& region_volume,
      const std::map<std::string, PhzDataModel::RegionResults>& result_map,
      const std::map<size_t, double>& region_cell_volume,
      const std::map<size_t,  std::vector<posterior_cell>>& region_cells,
      const std::map<size_t, std::string>& region_index_map);

};

} // end of namespace PhzOutput
} // end of namespace Euclid


#include "PhzOutput/_impl/LikelihoodHandler.icpp"

#endif	/* PHZOUTPUT_LIKELIHOODHANDLER_H */

