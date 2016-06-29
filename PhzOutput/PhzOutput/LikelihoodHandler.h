/**
 * @file LikelihoodHandler.h
 * @date March 13, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZOUTPUT_LIKELIHOODHANDLER_H
#define	PHZOUTPUT_LIKELIHOODHANDLER_H

#include <boost/filesystem.hpp>
#include "PhzDataModel/SourceResults.h"
#include "PhzOutput/OutputHandler.h"

namespace Euclid {
namespace PhzOutput {

template<PhzDataModel::SourceResultType GridMap>
class LikelihoodHandler : public OutputHandler {

public:

  LikelihoodHandler(boost::filesystem::path out_dir);

  virtual ~LikelihoodHandler() = default;

  void handleSourceOutput(const SourceCatalog::Source& source,
                          const PhzDataModel::SourceResults& results) override;


private:

  boost::filesystem::path m_out_dir;

};

} // end of namespace PhzOutput
} // end of namespace Euclid


#include "PhzOutput/_impl/LikelihoodHandler.icpp"

#endif	/* PHZOUTPUT_LIKELIHOODHANDLER_H */

