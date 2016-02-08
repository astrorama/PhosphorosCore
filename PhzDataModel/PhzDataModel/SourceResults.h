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
  /// The names of the parameter space regions
  REGION_NAMES,
  /// An map containing the the iterators to the best match model photometries
  /// for each parameter space region
  REGION_BEST_MODEL_ITERATOR,
  /// An iterator pointing to the model photometry which is the overall best match
  BEST_MODEL_ITERATOR,
  /// A map containing the grids representing the 1D PDF over the redshift for
  /// each parameter space region
  REGION_Z_1D_PDF,
  /// A grid representing the 1D PDF over the redshift
  Z_1D_PDF,
  /// A map containing the likelihood (before priors) for each parameter space region
  REGION_LIKELIHOOD,
  /// A map containing the posterior for each parameter space region
  REGION_POSTERIOR,
  /// A map containing the scale factor of the best match model photometry
  /// for each parameter space region
  REGION_BEST_MODEL_SCALE_FACTOR,
  /// A double with the value of the scale factor of the overall best match
  /// model photometry
  BEST_MODEL_SCALE_FACTOR,
  /// A map containing the natural logarithm of the normalization of the likelihood
  /// grid for each parameter space region
  REGION_LIKELIHOOD_NORM_LOG,
  /// A map containing the natural logarithm of the normalization of the posterior
  /// grid for each parameter space region
  REGION_POSTERIOR_NORM_LOG,
  /// A double with the chi square value of the overall best fitted model
  BEST_MODEL_CHI_SQUARE
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
  
  template <SourceResultType T, typename... Args>
  typename SourceResultTypeTraits<T>::type& setResult(Args&&... args);
  
  template <SourceResultType T>
  const typename SourceResultTypeTraits<T>::type& getResult() const;
  
  template <SourceResultType T>
  typename SourceResultTypeTraits<T>::type& getResult();

private:
  
  std::map<SourceResultType, boost::any> result_map {};

}; /* End of SourceResults class */

} /* namespace PhzDataModel */
} /* namespace Euclid */

#include "_impl/SourceResults.icpp"
#include "_impl/SourceResultTypeTraits.icpp"

#endif
