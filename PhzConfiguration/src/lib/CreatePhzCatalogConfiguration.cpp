/** 
 * @file CreatePhzCatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Exception.h"
#include "PhzOutput/BestModelCatalog.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"
#include "PhzOutput/LikelihoodHandler.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description CreatePhzCatalogConfiguration::getProgramOptions() {
  po::options_description options {"Create PHZ Catalog options"};

  options.add_options()
  ("output-catalog-file", po::value<std::string>(),
      "The filename of the file to export the PHZ catalog file")
  ("output-pdf-file", po::value<std::string>(),
        "The filename of the PDF data")
  ("marginalization-type", po::value<std::string>(),
        "The type of marginalization algorithm (one of SUM, MAX)")
  ("output-likelihood-dir", po::value<std::string>(),
        "The directory where the likelihood grids are stored");

  options.add(PhotometricCorrectionConfiguration::getProgramOptions());
  options.add(PhotometryCatalogConfiguration::getProgramOptions());
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

CreatePhzCatalogConfiguration::CreatePhzCatalogConfiguration(const std::map<std::string, po::variable_value>& options)
          : CatalogConfiguration(options), PhotometricCorrectionConfiguration(options),
            PhotometryCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  m_options = options;
}

class MultiOutputHandler : public PhzOutput::OutputHandler {
public:
	virtual ~MultiOutputHandler() = default;
	void addHandler(std::unique_ptr<PhzOutput::OutputHandler> handler) {
	  m_handlers.emplace_back(std::move(handler));
	}
	void handleSourceOutput(const SourceCatalog::Source& source,
	                                  const result_type& results) override {
		for (auto& handler : m_handlers) {
			handler->handleSourceOutput(source, results);
		}
	}
private:
	std::vector<std::unique_ptr<PhzOutput::OutputHandler>> m_handlers;
};

std::unique_ptr<PhzOutput::OutputHandler> CreatePhzCatalogConfiguration::getOutputHandler() {
  std::unique_ptr<MultiOutputHandler> result {new MultiOutputHandler{}};
  if (!m_options["output-catalog-file"].empty()) {
    std::string out_file = m_options["output-catalog-file"].as<std::string>();
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::BestModelCatalog{out_file}});
  }
  if (!m_options["output-pdf-file"].empty()) {
    std::string out_file = m_options["output-pdf-file"].as<std::string>();
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::PdfOutput{out_file}});
  }
  if (!m_options["output-likelihood-dir"].empty()) {
    std::string out_dir = m_options["output-likelihood-dir"].as<std::string>();
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::LikelihoodHandler{out_dir}});
  }
  return std::unique_ptr<PhzOutput::OutputHandler>{result.release()};
}

PhzLikelihood::SourcePhzFunctor::MarginalizationFunction CreatePhzCatalogConfiguration::getMarginalizationFunc() {
  if (m_options["marginalization-type"].empty()) {
    throw Elements::Exception() << "Missing mandatory parameter marginalization-type";
  }
  if (m_options["marginalization-type"].as<std::string>() == "SUM") {
    return PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  if (m_options["marginalization-type"].as<std::string>() == "MAX") {
    return PhzLikelihood::MaxMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  throw Elements::Exception() << "Unknown marginalization type \"" 
                    << m_options["marginalization-type"].as<std::string>() << "\"";
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
