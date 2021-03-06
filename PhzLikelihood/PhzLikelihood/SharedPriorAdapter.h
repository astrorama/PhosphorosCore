/**
 * @file PhzLikelihood/SharedPriorAdapter.h
 * @date Nov 4, 2015
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_
#define PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_

#include "PhzDataModel/RegionResults.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class SharedPriorAdapter
 *
 * @brief Adapter allowing to use a Prior which could not be copied as
 * a std::function and thus use it as a PriorFunction
 *
 * @tparam Prior The prior to be adapted. It must define a operator() with
 * the same signature as the SharedPriorAdapter.
 */
template <typename Prior>
class SharedPriorAdapter {
public:
  /**
   * @brief Constructor
   *
   * @param prior a shared_ptr on the prior to be adapted.
   */
  SharedPriorAdapter(std::shared_ptr<Prior> prior);

  /// Convenient factory, which creates an instance of the prior to be
  /// addapted by perfect forwarding the given arguments
  template <typename... PriorArgs>
  static SharedPriorAdapter<Prior> factory(PriorArgs&&... prior_args);

  /**
   * @brief destructor
   */
  virtual ~SharedPriorAdapter() = default;

  /**
   * @brief copy constructor
   *
   * @param other The SharedPriorAdapter to be copied.
   */
  SharedPriorAdapter(const SharedPriorAdapter& other) = default;

  /**
   * @brief copy assignment operator
   *
   * @param other The SharedPriorAdapter to be assigned.
   */
  SharedPriorAdapter& operator=(const SharedPriorAdapter& other) = default;

  /**
   * @brief function call operator. Has the signature to be used as a PriorFunction.
   */
  void operator()(PhzDataModel::RegionResults& results) const;

private:
  std::shared_ptr<Prior> m_prior;
};

}  // namespace PhzLikelihood
}  // namespace Euclid

#include "PhzLikelihood/_impl/SharedPriorAdapter.icpp"

#endif /* PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_ */
