/**
 * @file SedAxisCorrection.cpp
 * @date August 26, 2015
 * @author Florian Dubath
 */

#include "PhzLikelihood/SedAxisCorrection.h"

namespace Euclid {
namespace PhzLikelihood {

SedAxisCorrection::SedAxisCorrection(PhzLuminosity::SedGroupManager group_manager)
  :m_group_manager{std::move(group_manager)}{}

void SedAxisCorrection::operator()(PhzDataModel::LikelihoodGrid& likelihood_grid){

    auto axis = likelihood_grid.getAxis<PhzDataModel::ModelParameter::SED>();
    if (axis.size() <= 1) {
      return;
    }


    std::vector<double> weights {};


    for (size_t i=0; i<axis.size(); ++i) {
      weights.push_back(m_group_manager.getSedWeight(axis[i]));
    }

    // Apply the weights in each slice of the likelihood grid
    for (size_t i=0; i<axis.size(); ++i) {
      double w = weights[i];
      for (auto iter=likelihood_grid.begin().fixAxisByIndex<PhzDataModel::ModelParameter::SED>(i); iter!=likelihood_grid.end(); ++iter) {
        *iter = *iter * w;
      }
    }


}

}
}

