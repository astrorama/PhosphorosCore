/**
 * @file PhzModeling/PhotometryGridCreator.h
 * @date Oct 17, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_PHOTOMETRYGRIDCREATOR_H
#define PHZMODELING_PHOTOMETRYGRIDCREATOR_H

#include <functional>
#include "MathUtils/function/Function.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzModeling/ModelDatasetGrid.h"

namespace Euclid {

namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::PhotometryGridCreator
 * @brief
 * this class is the algorithm unit which is responsible for calculating the
 * Photometries of the models.
 *
 */
class PhotometryGridCreator {
  
public:
  
  typedef ModelDatasetGrid::IgmAbsorptionFunction IgmAbsorptionFunction;
  
  /**
   * Defines the signature of the functions which can be used as listeners for
   * the progress of the photometry grid creation. The first parameter is the
   * number of the current step and the second is the total number of steps.
   */
  typedef std::function<void(size_t step, size_t total)> ProgressListener;
  
  /**
   * @brief constructor
   *
   * @details
   * Instanciate and initialize a PhotometryGridCreator.
   *
   * @param sed_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the SEDs
   *
   * @param reddening_curve_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the Reddening curves
   *
   * @param filter_provider
   * A shared_ptr<XYDatasetProvider> allowing to access to the Filters
   * 
   * @param igm_absorption_function
   * The function to use for applying the IGM absorption to the redshifted SED
   *
   */
  PhotometryGridCreator(
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
      std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider,
      IgmAbsorptionFunction igm_absorption_function);
  /**
   * @brief destructor.
   */
  virtual ~PhotometryGridCreator();

  /**
   * @brief Creates a photometry grid
   * @details
   * Build a photometry grid. If the parameter space requires
   * a SED or a reddening curve that the corresponding provider are unable to returns
   * an exception will be throw an Euclid Exception. An exception is also throw if the
   * filter provider cannot provide the requested filters.
   *
   * @param parameter_space
   * A ModelAxesTuple defining the SEDs, the redshifts, the reddening curves and the EVB values
   * for which the photometry will be computed.
   *
   * @param filter_name_list
   * A vector<QualifiedName> containing the list (and order) of filter for building the photometry.
   * 
   * @param progress_listener
   * A function of type ProgressListener which will be updated with the progress
   * of the grid creation. It will be called every 0.1 sec. The default is an
   * empty function, which means no action is taken.
   *
   */
  PhzDataModel::PhotometryGrid createGrid(const PhzDataModel::ModelAxesTuple& parameter_space,
              const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list,
              ProgressListener progress_listener=ProgressListener{});

private:

  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_sed_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_reddening_curve_provider;
  std::shared_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  IgmAbsorptionFunction m_igm_absorption_function;

};

}
}

#endif    /* PHZMODELING_PHOTOMETRYGRIDCREATOR_H */
