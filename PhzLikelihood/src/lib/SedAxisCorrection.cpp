/**
 * @file SedAxisCorrection.cpp
 * @date August 26, 2015
 * @author Florian Dubath
 */

#include "PhzLikelihood/SedAxisCorrection.h"

namespace Euclid {
namespace PhzLikelihood {

SedAxisCorrection::SedAxisCorrection(PhzDataModel::QualifiedNameGroupManager group_manager)
  :m_group_manager{std::move(group_manager)}{}

void SedAxisCorrection::operator()(PhzDataModel::LikelihoodGrid& likelihood_grid){

    auto axis = likelihood_grid.getAxis<PhzDataModel::ModelParameter::SED>();
    if (axis.size() <= 1) {
      return;
    }


    std::vector<double> weights {};


    for (size_t i=0; i<axis.size(); ++i) {
      weights.push_back(1. / m_group_manager.findGroupContaining(axis[i]).second.size());
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

