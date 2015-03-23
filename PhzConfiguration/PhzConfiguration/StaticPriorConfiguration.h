/** 
 * @file StaticPriorConfiguration.h
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_STATICPRIORCONFIGURATION_H
#define	PHZCONFIGURATION_STATICPRIORCONFIGURATION_H

#include <vector>
#include <boost/program_options.hpp>
#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzConfiguration {

class StaticPriorConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  virtual ~StaticPriorConfiguration() = default;
  
  std::vector<PhzLikelihood::CatalogHandler::StaticPriorFunction> getStaticPriors();
  
protected:
  
  virtual void addStaticPrior(PhzLikelihood::CatalogHandler::StaticPriorFunction prior) final;
  
private:
  
  std::vector<PhzLikelihood::CatalogHandler::StaticPriorFunction> m_static_priors {};
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_STATICPRIORCONFIGURATION_H */

