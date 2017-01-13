/*
 * PdfOutput.h
 *
 *  Created on: Feb 4, 2015
 *      Author: Nicolas Morisset
 */

#ifndef PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_
#define PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_


#include <boost/filesystem.hpp>
#include <CCfits/CCfits>
#include "Table/Row.h"
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

/**
 * @class PdfOutput
 *
 * @brief
 * This class which inherits from the OutputHandler class is responsible for writing the 1D PDFs into a FITS file
 * @details
 * All 1D PDFs of a source are stored in the same FITS extension, so one source, one extension.
 * The handleSourceOutput function is called per source. The FITS filename(included path) is given to the constructor.
 * In order to keep the writing as much as possible efficient(from memory and cpu) we close and reopen the file
 * every 5000 sources. For the moment, this number is hard coded.
 *
 */

class PdfOutput : public OutputHandler {

public:

  /**
   * @brief Constructor
   * The filename is passed to the constructor
   * @details
   *
   * @param out_file
   * The path and filename of the FITS file of type boost::filesystem::path
   *
   */
//	PdfOutput(boost::filesystem::path out_file) :m_out_file{out_file},
//	                              m_fits_file{new CCfits::FITS{"!"+out_file.string(), CCfits::RWmode::Write}},
//	                              m_counter(0) {}

	PdfOutput(boost::filesystem::path out_file);

  virtual ~PdfOutput();

  /**
   * @brief
   * This function puts all 1D PDFs of a source into a FITS extension
   * @param source
   * A Source object
   * @param results
   *    The results of the template fitting for the source
   */
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results);

private:

  boost::filesystem::path       m_out_file;  // Filename of the output fits file
  std::unique_ptr<CCfits::FITS> m_fits_file; // Unique pointer to the FITS file object
  int64_t                       m_counter;   // Counting the number of sources

};

} // end of namespace PhzOutput
} // end of namespace Euclid



#endif /* PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_ */
