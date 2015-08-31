/**
 * @file ComputeLuminosityModelGridConfiguration.h
 * @date Août 31, 2015
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_COMPUTELUMINOSITYMODELGRIDCONFIGURATION_H
#define	PHZCONFIGURATION_COMPUTELUMINOSITYMODELGRIDCONFIGURATION_H

#include <functional>
#include <map>
#include <string>

#include <boost/program_options.hpp>

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzConfiguration/LuminosityTypeConfiguration.h"
#include "PhzConfiguration/CatalogTypeConfiguration.h"
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

class ComputeLuminosityModelGridConfiguration : public ParameterSpaceConfiguration,
                                      public LuminosityTypeConfiguration,
                                      public CatalogTypeConfiguration{

public:

  typedef std::function<void(const std::map<std::string, PhzDataModel::PhotometryGrid>&)> OutputFunction;

  /**
   * @brief
   * Get program options of the ComputeModelGridConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();

  ComputeLuminosityModelGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

  virtual ~ComputeLuminosityModelGridConfiguration()=default;


  OutputFunction getOutputFunction();

  std::vector<XYDataset::QualifiedName> getLuminosityFilterList();

  std::map<std::string, PhzDataModel::ModelAxesTuple> getLuminosityParameterSpaceRegions();

  std::unique_ptr<XYDataset::XYDatasetProvider> getFilterDatasetProvider();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_COMPUTELUMINOSITYMODELGRIDCONFIGURATION_H */

