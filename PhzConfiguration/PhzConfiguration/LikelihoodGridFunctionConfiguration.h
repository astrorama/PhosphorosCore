/* 
 * @file LikelihoodGridFunctionConfiguration.h
 *
 * Created on: September 24, 2015
 *     Author: Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIGURATION_H
#define	PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIGURATION_H

#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzConfiguration {

class LikelihoodGridFunctionConfiguration : virtual public PhotometryCatalogConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  LikelihoodGridFunctionConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);
  
  virtual ~LikelihoodGridFunctionConfiguration() = default;
  
  PhzLikelihood::SourcePhzFunctor::LikelihoodGridFunction getLikelihoodGridFunction();
  
private:
  
  bool m_enable_missing_data = false;
  bool m_enable_upper_limit = false;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid



#endif	/* PHZCONFIGURATION_LIKELIHOODGRIDFUNCTIONCONFIGURATION_H */

