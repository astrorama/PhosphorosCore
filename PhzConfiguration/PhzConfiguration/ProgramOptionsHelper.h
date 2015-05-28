/** 
 * @file ProgramOptionsHelper.h
 * @date May 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PROGRAMOPTIONSHELPER_H
#define	PHZCONFIGURATION_PROGRAMOPTIONSHELPER_H

#include <vector>
#include <string>
#include <boost/program_options.hpp>

namespace Euclid {
namespace PhzConfiguration {

class UniqueOptionsMerger {
  
public:
  
  UniqueOptionsMerger(const boost::program_options::options_description& options) : m_options(options) { }
  
  operator boost::program_options::options_description() const {
    return m_options;
  }
  
  UniqueOptionsMerger operator()(const boost::program_options::options_description& options) const;
  
private:
  
  boost::program_options::options_description m_options {};
  
};

UniqueOptionsMerger merge(const boost::program_options::options_description& options);

}
}

#endif	/* PHZCONFIGURATION_PROGRAMOPTIONSHELPER_H */

