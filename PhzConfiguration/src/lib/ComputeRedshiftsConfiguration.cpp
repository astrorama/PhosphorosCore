/**
 * @file ComputeRedshiftsConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "ElementsKernel/Exception.h"
#include "PhzOutput/BestModelCatalog.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzLikelihood/SumMarginalizationFunctor.h"
#include "PhzLikelihood/MaxMarginalizationFunctor.h"
#include "PhzLikelihood/BayesianMarginalizationFunctor.h"
#include "PhzConfiguration/ComputeRedshiftsConfiguration.h"
#include "PhzOutput/LikelihoodHandler.h"
#include "CheckPhotometries.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description ComputeRedshiftsConfiguration::getProgramOptions() {
  po::options_description options {"Fit Templates options"};

  options.add_options()
  ("output-catalog-file", po::value<std::string>(),
      "The filename of the file to export the PHZ catalog file")
  ("output-catalog-format", po::value<std::string>(),
      "The format of the PHZ catalog file (one of ASCII (default), FITS)")
  ("output-pdf-file", po::value<std::string>(),
        "The filename of the PDF data")
  ("marginalization-type", po::value<std::string>(),
        "The type of marginalization algorithm (one of SUM, MAX, BAYESIAN)")
        ("output-likelihood-dir", po::value<std::string>(),
              "The directory where the likelihood grids are stored");

  options.add(PhotometricCorrectionConfiguration::getProgramOptions());
  options.add(PhotometryCatalogConfiguration::getProgramOptions());
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

ComputeRedshiftsConfiguration::ComputeRedshiftsConfiguration(const std::map<std::string, po::variable_value>& options)
          : PriorConfiguration(), CatalogConfiguration(options), PhotometricCorrectionConfiguration(options),
            PhotometryCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  m_options = options;

  // Check that the given grid contains photometries for all the filters we
  // have fluxes in the catalog
  if (!m_options["filter-name-mapping"].empty()) {
    checkGridPhotometriesMatch(getPhotometryGridInfo().filter_names,
                               m_options["filter-name-mapping"].as<std::vector<std::string>>());
  }

  // Check that we have photometric corrections for all the filters
  if (!m_options["filter-name-mapping"].empty()) {
    checkHaveAllCorrections(getPhotometricCorrectionMap(),
                            m_options["filter-name-mapping"].as<std::vector<std::string>>());
  }
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

std::unique_ptr<PhzOutput::OutputHandler> ComputeRedshiftsConfiguration::getOutputHandler() {
  std::unique_ptr<MultiOutputHandler> result {new MultiOutputHandler{}};
  if (!m_options["output-catalog-file"].empty()) {
    std::string out_file = m_options["output-catalog-file"].as<std::string>();
    auto format = PhzOutput::BestModelCatalog::Format::ASCII;
    if (!m_options["output-catalog-format"].empty()) {
      auto format_str = m_options["output-catalog-format"].as<std::string>();
      if (format_str == "ASCII") {
        format = PhzOutput::BestModelCatalog::Format::ASCII;
      } else if (format_str == "FITS") {
        format = PhzOutput::BestModelCatalog::Format::FITS;
      } else {
        throw Elements::Exception() << "Unknown output catalog format " << format_str;
      }
    }
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::BestModelCatalog{out_file, format}});
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

PhzLikelihood::SourcePhzFunctor::MarginalizationFunction ComputeRedshiftsConfiguration::getMarginalizationFunc() {
  if (m_options["marginalization-type"].empty()) {
    throw Elements::Exception() << "Missing mandatory parameter marginalization-type";
  }
  if (m_options["marginalization-type"].as<std::string>() == "SUM") {
    return PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  if (m_options["marginalization-type"].as<std::string>() == "MAX") {
    return PhzLikelihood::MaxMarginalizationFunctor<PhzDataModel::ModelParameter::Z>{};
  }
  if (m_options["marginalization-type"].as<std::string>() == "BAYESIAN") {
    return PhzLikelihood::BayesianMarginalizationFunctor{};
  }
  throw Elements::Exception() << "Unknown marginalization type \""
                    << m_options["marginalization-type"].as<std::string>() << "\"";
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
