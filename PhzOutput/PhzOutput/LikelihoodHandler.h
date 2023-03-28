/**
 * @file LikelihoodHandler.h
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_LIKELIHOODHANDLER_H
#define PHZOUTPUT_LIKELIHOODHANDLER_H

#include "PhzDataModel/RegionResults.h"
#include "PhzDataModel/PPConfig.h"
#include "PhzOutput/GridSampler.h"
#include "PhzOutput/OutputHandler.h"
#include <boost/filesystem.hpp>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace Euclid {
namespace PhzOutput {

template <PhzDataModel::RegionResultType GridType, typename Sampler>
class LikelihoodHandler : public OutputHandler {

public:
  LikelihoodHandler(
      boost::filesystem::path out_dir,
      const std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>>& param_config,
      bool do_sample,
	  size_t sample_number = 1000,
	  size_t chunk_size = 10000);

  virtual ~LikelihoodHandler();

  void handleSourceOutput(const SourceCatalog::Source& source, const PhzDataModel::SourceResults& results) override;

private:
  boost::filesystem::path                                                                      m_out_dir;
  const std::map<std::string, std::map<std::string, PhzDataModel::PPConfig>>& m_param_config;

  bool   m_do_sample;
  size_t m_sample_number;
  size_t m_chunk_size;

  size_t                                     m_current_file_id          = 0;
  bool                                       m_current_file_has_comment = false;
  boost::filesystem::path                    m_current_out_file;    // Filename of the output fits file
  std::shared_ptr<CCfits::FITS>              m_current_fits_file;   // Pointer to the FITS file object
  std::shared_ptr<Table::FitsWriter>         m_sample_fits_writer;  // table writer around the fits file objects
  std::shared_ptr<Euclid::Table::ColumnInfo> m_sampling_column_info;
  size_t                                     m_counter;  // Counting the number of sources

  boost::filesystem::path                    m_index_file;       // Filename of the output fits file
  std::shared_ptr<CCfits::FITS>              m_index_fits_file;  // Pointer to the FITS file object
  std::shared_ptr<Euclid::Table::ColumnInfo> m_index_column_info;
  std::vector<Table::Row>                    m_index_row_list{};
  std::vector<Table::Row>                    m_sample_row_list{};
  Sampler                                    m_grid_sampler;

  std::string getFileName(int file_id);
  void        createColumnsLists(const SourceCatalog::Source& first_source);
  void        createAndOpenSampleFile();
  void        closeSampleFile();

  void exportFullGrid(const SourceCatalog::Source& source, const PhzDataModel::SourceResults& results);

  std::vector<Table::Row> drawSample(SourceCatalog::Source::id_type                            source_id,
                                     const std::map<std::string, double>&                      region_volume,
                                     const std::map<std::string, PhzDataModel::RegionResults>& result_map);
};

}  // end of namespace PhzOutput
}  // end of namespace Euclid

#include "PhzOutput/_impl/LikelihoodHandler.icpp"

#endif /* PHZOUTPUT_LIKELIHOODHANDLER_H */
