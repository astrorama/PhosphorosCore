/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */  

/**
 * @file PhzDataModel/SourceResults.h
 * @date 01/14/16
 * @author nikoapos
 */

#ifndef _PHZDATAMODEL_SOURCERESULTS_H
#define _PHZDATAMODEL_SOURCERESULTS_H

#include <functional>
#include <map>
#include <boost/any.hpp>

namespace Euclid {
namespace PhzDataModel {


/// An enumeration representing the available PHZ source results
enum class SourceResultType {
  /// An iterator pointing to the model photometry which is the best match
  BEST_MODEL_ITERATOR,
  /// A grid representing the 1D PDF over the redshift
  Z_1D_PDF,
  /// A map containing the likelihood (before priors) for all parameter space regions
  LIKELIHOOD,
  /// A map containing the posterior for all parameter space regions
  POSTERIOR,
  /// A double with the value of the alpha scale factor used for the fitting
  SCALE_FACTOR,
  /// A double with the chi square value of the best fitted model
  BEST_MODEL_CHI_SQUARE,
  /// A map containing the best chi square for each parameter space region
  BEST_CHI_SQUARE_MAP
};


template <SourceResultType T, typename=void>
struct SourceResultTypeTraits;


/**
 * @class SourceResults
 * @brief
 *
 */
class SourceResults {

public:

  /**
   * @brief Destructor
   */
  virtual ~SourceResults() = default;
  
  template <SourceResultType T>
  void setResult(typename SourceResultTypeTraits<T>::type result);
  
  template <SourceResultType T>
  const typename SourceResultTypeTraits<T>::type& getResult() const;

private:
  
  std::map<SourceResultType, boost::any> result_map {};

}; /* End of SourceResults class */

} /* namespace PhzDataModel */
} /* namespace Euclid */

#include "_impl/SourceResults.icpp"
#include "_impl/SourceResultTypeTraits.icpp"

#endif
