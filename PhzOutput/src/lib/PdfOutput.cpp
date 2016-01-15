/*
 * PdfOutput.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: Nicolas Morisset
 */

#include <fstream>
#include <string>
#include <vector>
#include "ElementsKernel/Logging.h"
#include "Table/Table.h"
#include "Table/FitsWriter.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzUtils/FileUtils.h"


namespace Euclid {
namespace PhzOutput {

static Elements::Logging logger = Elements::Logging::getLogger("PhzOutput");

PdfOutput::PdfOutput(boost::filesystem::path out_file): m_out_file(out_file), m_counter (0) {

  // Check directory and write permissions
  Euclid::PhzUtils::checkCreateDirectoryWithFile(out_file.string());

  m_fits_file.reset(new CCfits::FITS{"!"+out_file.string(), CCfits::RWmode::Write});

}


PdfOutput::~PdfOutput() {
  logger.info() << "Created 1D PDFs in file " << m_out_file.string();
}

void PdfOutput::handleSourceOutput(const SourceCatalog::Source& source,
                                   const result_type& results) {
  // Count the number of sources saved
  ++m_counter;

  // Create the columnInfo object
  std::shared_ptr<Euclid::Table::ColumnInfo> column_info {new Euclid::Table::ColumnInfo {{
	  Euclid::Table::ColumnInfo::info_type("Z", typeid(double)),
	  Euclid::Table::ColumnInfo::info_type("Probability", typeid(double))
  }}};

  // Transfer pdf data to rows
  std::vector<Table::Row> row_list {};
  auto& pdf = results.getResult<PhzDataModel::SourceResultType::Z_1D_PDF>();
  for (auto iter=pdf.begin(); iter!=pdf.end(); ++iter) {
	  row_list.push_back(Table::Row{{iter.axisValue<0>(), *iter}, column_info});
  }

  Table::Table pdf_table{row_list};

  // Store pdf data into fits file
  Table::FitsWriter fits_writer {Table::FitsWriter::Format::BINARY};

  // Close and reopen the FITS object for efficiency reason
	if (m_counter % 5000 == 0) {
		m_fits_file->destroy();
		m_fits_file.reset( new CCfits::FITS {m_out_file.string(), CCfits::RWmode::Write});
		m_counter = 0;
	}

  fits_writer.write(*m_fits_file, std::to_string(source.getId()), pdf_table);
}

} // end of namespace PhzOutput
} // end of namespace Euclid


