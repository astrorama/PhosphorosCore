/**
 * @file LuminosityPriorConfiguration.h
 * @date August 21, 2015
 * @author dubathf
 */

#ifndef PHZCONFIGURATION_LUMINOSITYPRIORCONFIGURATION_H
#define	PHZCONFIGURATION_LUMINOSITYPRIORCONFIGURATION_H

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "PhzLuminosity/LuminosityPrior.h"
#include "PhzConfiguration/LuminosityFunctionConfiguration.h"
#include "PhzConfiguration/LuminositySedGroupConfiguration.h"
#include "PhzConfiguration/CatalogTypeConfiguration.h"
#include "PhzConfiguration/PriorConfiguration.h"
namespace Euclid {
namespace PhzConfiguration {

class LuminosityPriorConfiguration: public virtual LuminosityTypeConfiguration,
                                    public virtual PhosphorosPathConfiguration,
                                    public virtual LuminosityFunctionConfiguration,
                                    public virtual LuminositySedGroupConfiguration,
                                    public virtual CatalogTypeConfiguration,
                                    public virtual PriorConfiguration{

public:

  static boost::program_options::options_description getProgramOptions();

  LuminosityPriorConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

   virtual ~LuminosityPriorConfiguration()=default;

   bool DoApplyLuminosityPrior();

   PhzDataModel::PhotometryGrid getLuminosityModelGrid();


private:

  std::map<std::string, boost::program_options::variable_value> m_options;


};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_LUMINOSITYPRIORCONFIGURATION_H */

