/** 
 * @file LikelihoodHandler.h
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_LIKELIHOODHANDLER_H
#define	PHZOUTPUT_LIKELIHOODHANDLER_H

#include <boost/filesystem.hpp>
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

class LikelihoodHandler : public OutputHandler {
  
public:
  
  LikelihoodHandler(boost::filesystem::path out_dir);
  
  virtual ~LikelihoodHandler() = default;
  
  void handleSourceOutput(const SourceCatalog::Source& source,
                          const result_type& results) override;

  
private:
  
  boost::filesystem::path m_out_dir;
  
};

} // end of namespace PhzOutput
} // end of namespace Euclid

#endif	/* PHZOUTPUT_LIKELIHOODHANDLER_H */

