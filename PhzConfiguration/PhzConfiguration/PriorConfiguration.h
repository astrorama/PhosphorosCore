/** 
 * @file PriorConfiguration.h
 * @date March 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PRIORCONFIGURATION_H
#define	PHZCONFIGURATION_PRIORCONFIGURATION_H

#include <vector>
#include <boost/program_options.hpp>
#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzConfiguration {

class PriorConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  virtual ~PriorConfiguration() = default;
  
  std::vector<PhzLikelihood::CatalogHandler::PriorFunction> getPriors();
  
protected:
  
  virtual void addPrior(PhzLikelihood::CatalogHandler::PriorFunction prior) final;
  
private:
  
  std::vector<PhzLikelihood::CatalogHandler::PriorFunction> m_priors {};
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PRIORCONFIGURATION_H */

