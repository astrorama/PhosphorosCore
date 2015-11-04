/**
 * @file PhzLikelihood/SharedPriorAdapter.h
 * @date Nov 4, 2015
 * @author Florian Dubath
 */

#ifndef PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_
#define PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/ScaleFactorGrid.h"


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
template<typename Prior>
class SharedPriorAdapter{
public:
  /**
   * @brief Constructor
   *
   * @param prior a shared_ptr on the prior to be adapted.
   */
  SharedPriorAdapter(std::shared_ptr<Prior> prior);

  /**
   * @brief destructor
   */
  virtual ~SharedPriorAdapter()=default;

  /**
   * @brief copy constructor
   *
   * @param other The SharedPriorAdapter to be copied.
   */
  SharedPriorAdapter ( const SharedPriorAdapter & other)=default;

  /**
   * @brief copy assignment operator
   *
   * @param other The SharedPriorAdapter to be assigned.
   */
  SharedPriorAdapter & operator= ( const SharedPriorAdapter & other)=default;

  /**
   * @brief function call operator. Has the signature to be used as a PriorFunction.
   */
  void operator()(PhzDataModel::LikelihoodGrid& likelihoodGrid,
        const SourceCatalog::Photometry& sourcePhotometry,
        const PhzDataModel::PhotometryGrid& modelGrid,
        const PhzDataModel::ScaleFactordGrid& scaleFactorGrid) const;

private:
  std::shared_ptr<Prior> m_prior;
};

}
}


#include "PhzLikelihood/_impl/SharedPriorAdapter.icpp"

#endif /* PHZLIKELIHOOD_PHZLIKELIHOOD_SHAREDPRIORADAPTER_H_ */
