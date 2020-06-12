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

  LikelihoodHandler(boost::filesystem::path out_dir, bool do_sample, size_t sample_number = 1000, size_t flush_chunk_size = 500);

  virtual ~LikelihoodHandler();

  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override;


private:

  boost::filesystem::path m_out_dir;
  bool m_do_sample;
  size_t m_sample_number;
  size_t m_flush_chunk_size;
  int64_t m_counter;   // Counting the number of sources

  boost::filesystem::path       m_out_file;  // Filename of the output fits file
  std::shared_ptr<CCfits::FITS> m_fits_file; // Pointer to the FITS file object
};

} // end of namespace PhzOutput
} // end of namespace Euclid


#include "PhzOutput/_impl/LikelihoodHandler.icpp"

#endif	/* PHZOUTPUT_LIKELIHOODHANDLER_H */

