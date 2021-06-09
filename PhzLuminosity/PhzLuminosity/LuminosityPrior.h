/*
 * LuminosityPrior.h
 *
 *  Created on: Aug 6, 2015
 *      Author: fdubath
 */

#ifndef PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_
#define PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_

#include <PhzLuminosity/LuminosityFunctionSet.h>
#include <vector>
#include <memory>

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhysicsUtils/CosmologicalParameters.h"

#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/RegionResults.h"

#include "PhzDataModel/QualifiedNameGroupManager.h"

namespace Euclid {
namespace PhzLuminosity {


class LuminosityPrior{
public:

  class LuminosityGroupSampledProcessor {
  public:
    LuminosityGroupSampledProcessor(PhzDataModel::DoubleListGrid& prior_scal_grid,
                             const PhzDataModel::DoubleGrid& scale_factor_grid,
                             const PhzDataModel::DoubleGrid& sigma_scale_factor_grid,
                             bool in_mag,
                             double caling_sigma_range,
                             const size_t sample_number);

    void operator()(const std::function<double(double)>& luminosity_funct, size_t sed_index, size_t z_index);

    double getMaxPrior();
  private:
    PhzDataModel::DoubleListGrid& m_prior_scal_grid;
    const PhzDataModel::DoubleGrid& m_scale_factor_grid;
    const PhzDataModel::DoubleGrid& m_sigma_scale_factor_grid;
    const bool m_in_mag;
    const double m_scaling_sigma_range;
    const size_t m_sample_number;
    double m_max = 0.0;

  };

  class LuminosityGroupdProcessor {
  public:
    LuminosityGroupdProcessor(PhzDataModel::DoubleGrid& prior_grid,
                              const PhzDataModel::DoubleGrid& scale_factor_grid,
                              bool in_mag);

     void operator()(const std::function<double(double)>& luminosity_funct, size_t sed_index, size_t z_index);

     double getMaxPrior();
   private:
     PhzDataModel::DoubleGrid& m_prior_grid;
     const PhzDataModel::DoubleGrid& m_scale_factor_grid;
     const bool m_in_mag;
     double m_max = 0.0;
  };



LuminosityPrior(
    PhzDataModel::QualifiedNameGroupManager sedGroupManager,
    LuminosityFunctionSet luminosityFunctionSet,
    bool in_mag = true,
    double scaling_sigma_range = 5.,
    double effectiveness = 1.);

void operator()(PhzDataModel::RegionResults& results) const;

PhzDataModel::DoubleGrid createPriorGrid(const PhzDataModel::DoubleGrid& posterior_grid) const;
PhzDataModel::DoubleListGrid createListPriorGrid(const PhzDataModel::DoubleListGrid& posterior_grid) const;

void applyEffectiveness(PhzDataModel::DoubleGrid& prior_grid, double max) const;
void applySampleEffectiveness(PhzDataModel::DoubleListGrid& prior_grid, double max) const;

void applyPrior(PhzDataModel::DoubleGrid& prior_grid, PhzDataModel::DoubleGrid& posterior_grid) const;
void applySamplePrior(PhzDataModel::DoubleListGrid& prior_grid, PhzDataModel::DoubleListGrid& posterior_grid) const;


static double getMagFromFlux(double flux);

static double getLuminosityInSample(double alpha, double n_sigma, size_t sample_number, size_t sample_index);

template <typename Processor, typename Axis_SED, typename Axis_Z>
double fillTheGrid(Processor& processor, Axis_SED& sed_axis, Axis_Z& z_axis) const;

private:

  PhzDataModel::QualifiedNameGroupManager m_sed_group_manager;
  LuminosityFunctionSet m_luminosity_function_set;
  const bool m_in_mag;
  const double m_scaling_sigma_range;
  const double m_effectiveness;
};

}
}


#endif /* PHZLUMINOSITY_PHZLUMINOSITY_LUMINOSITYPRIOR_H_ */
