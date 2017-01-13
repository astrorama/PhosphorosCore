/*
 * MagnitudeForComparison.cpp
 *
 *  Created on: Feb 5, 2016
 *      Author: fdubath
 */

#include <map>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzLikelihood/SingleGridPhzFunctor.h"


#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"
#include "Configuration/Utils.h"
#include "PhzConfiguration/CatalogTypeConfig.h"
#include "PhzConfiguration/CatalogDirConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "Configuration/PhotometryCatalogConfig.h"



#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"
#include "PhzConfiguration/PhosphorosCatalogConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"


#include <boost/archive/binary_iarchive.hpp>
#include "GridContainer/serialize.h"
#include "PhzLuminosity/ReddenedLuminosityCalculator.h"


#include "PhysicsUtils/CosmologicalDistances.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"

#include "PhzTest/ModelFixingAttribute.h"
#include "PhzTest/ModelFixingAttributeFromRow.h"
#include "PhzTest/ModelFixingCatalogConfig.h"


#include "PhzLikelihood/LikelihoodLogarithmAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareLikelihoodLogarithm.h"

#include "PhzTest/LuminosityGridConfig.h"
#include "PhzTest/OutputFileConfig.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzPhotometricCorrection;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

static Elements::Logging logger = Elements::Logging::getLogger("MagnitudeForComparison");

using ResType = PhzDataModel::RegionResultType;

static long config_manager_id = getUniqueManagerId();

class MagnitudeForComparison: public Elements::Program {
public:

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    config_manager.registerConfiguration<PhosphorosRootDirConfig>();
    config_manager.registerConfiguration<CatalogTypeConfig>();
    config_manager.registerConfiguration<CatalogDirConfig>();
    config_manager.registerConfiguration<IntermediateDirConfig>();
    config_manager.registerConfiguration<CosmologicalParameterConfig>();
    config_manager.registerConfiguration<CatalogConfig>();
    config_manager.registerConfiguration<PhosphorosCatalogConfig>();
    config_manager.registerConfiguration<PhotometryGridConfig>();
    config_manager.registerConfiguration<PhzTest::ModelFixingCatalogConfig>();
    config_manager.registerConfiguration<PhzTest::LuminosityGridConfig>();
    config_manager.registerConfiguration<PhotometryCatalogConfig>();
    config_manager.registerConfiguration<PhzTest::OutputFileConfig>();



    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);

    config_manager.initialize(args);



    // Get the Luminosity grid file
    auto luminosity_model_grid = config_manager.getConfiguration<PhzTest::LuminosityGridConfig>().getLuminosityGrid();
    // Constraint: single region grid


    // build distances maps
    std::map<double,double> luminosity_distance_map{};
    std::map<double,double> distance_modulus_map{};

    auto& cosmological_param = config_manager.getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam() ;
    PhysicsUtils::CosmologicalDistances cosmological_distances {};
    for (auto& pair : config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo().region_axes_map) {
        for (auto& z_value : std::get<PhzDataModel::ModelParameter::Z>(pair.second)) {
           distance_modulus_map[z_value] = cosmological_distances.distanceModulus(z_value, cosmological_param);
           luminosity_distance_map[z_value] = cosmological_distances.luminousDistance(z_value, cosmological_param);
        }
    }

    distance_modulus_map[0] = cosmological_distances.distanceModulus(0, cosmological_param);
    luminosity_distance_map[0] = cosmological_distances.luminousDistance(0, cosmological_param);

    // create a Luminosity calculator for each filter
    std::vector<PhzLuminosity::ReddenedLuminosityCalculator> luminosity_calculators{};
    std::string columns_names = "# Id";
    for(auto& filter : config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo().filter_names){
      logger.info() << "Add the Filter :"<<filter.qualifiedName();
      columns_names += " "+filter.qualifiedName()+ " model_"+filter.qualifiedName()+" abs_"+filter.qualifiedName();
      luminosity_calculators.push_back(PhzLuminosity::ReddenedLuminosityCalculator{filter,
                                                                    luminosity_model_grid,
                                                                    luminosity_distance_map,
                                                                    distance_modulus_map,
                                                                    true});
    }

    // load the model grid
    const auto& model_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    if (model_grid.size()>1){
          logger.info() << "Test can not be performed with a Photometric grid with multiples regions.";
          return Elements::ExitCode::NOT_OK;
    }




    const auto& model_single_grid = model_grid.begin()->second;

    //logger.info() << model_single_grid.size();

    // read the catalog
    auto& catalog = config_manager.getConfiguration<CatalogConfig>().getCatalog();

    PhzLikelihood::SingleGridPhzFunctor::LikelihoodGridFunction likelihood_func =
        PhzLikelihood::LikelihoodGridFunctor{
      PhzLikelihood::LikelihoodLogarithmAlgorithm{
        PhzLikelihood::ScaleFactorFunctorSimple{},
        PhzLikelihood::ChiSquareLikelihoodLogarithmSimple{}}};


    auto out_file_name = config_manager.getConfiguration<PhzTest::OutputFileConfig>().getOutputFile().string();
    logger.info() << "output in :" << out_file_name;
    logger.info() << "-------------------------------------------------------------------------------";

    std::ofstream outputFile( out_file_name );

    logger.info() << columns_names;
    outputFile << columns_names << std::endl;


    // for each sources
    for(auto& source : catalog){
      auto source_photometry = source.getAttribute<SourceCatalog::Photometry>();
      if (source_photometry==nullptr){
        logger.info() << "null photometry";
      }
      
      //  compute the scale factor grid
      PhzDataModel::RegionResults results {};
      results.set<ResType::SOURCE_PHOTOMETRY_REFERENCE>(*source_photometry);
      results.set<ResType::MODEL_GRID_REFERENCE>(model_single_grid);
      likelihood_func(results);
      auto& scale_factor_grid = results.get<ResType::SCALE_FACTOR_GRID>();


      //  fix the iterator on the scale factor grid
      auto model_fixing = source.getAttribute<PhzTest::ModelFixingAttribute>();



      auto alpha_iter = scale_factor_grid.cbegin();

      alpha_iter.fixAxisByValue<PhzDataModel::ModelParameter::Z>(adjustZAxis(scale_factor_grid,model_fixing->getZ()));
      alpha_iter.fixAxisByValue<PhzDataModel::ModelParameter::REDDENING_CURVE>(model_fixing->getRedCurve());
      alpha_iter.fixAxisByValue<PhzDataModel::ModelParameter::EBV>(adjustEBVAxis(scale_factor_grid, model_fixing->getEbv()));
      alpha_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(model_fixing->getSed());

      std::vector<double> mags {};
      //  compute the luminosity for each filter
      std::string result_row = std::to_string(source.getId());
      for(auto& lum_calc : luminosity_calculators){
        auto band_flux = source_photometry->find(lum_calc.getLuminosityFilter().qualifiedName())->flux;
        double mag_app = computeApparentMag(band_flux);
        double magg_app_model=computeApparentMagFromModel(alpha_iter,luminosity_model_grid,model_fixing,lum_calc.getLuminosityFilter(),scale_factor_grid);

        double mag = lum_calc(alpha_iter);
        mags.push_back(mag);
        result_row+=" "+std::to_string(mag_app)+" "+std::to_string(magg_app_model)+" "+std::to_string(mag);
      }

     // logger.info() << result_row << " z:"<< std::to_string(model_fixing->getZ());

      outputFile << result_row << std::endl;

      // the vector with the mag is also available here...

    }



    logger.info() << "-------------------------------------------------------------------------------";




    logger.info() << "End of the Test";
    return Elements::ExitCode::OK;
  }

  double adjustZAxis(const PhzDataModel::DoubleGrid& grid, double z_val){
    auto axis= grid.getAxis<PhzDataModel::ModelParameter::Z>();
    for (auto& z_axis : axis){
      if (abs(z_axis-z_val)<0.00001){
        return z_axis;
      }
    }

    return z_val;
  }

  double adjustEBVAxis(const PhzDataModel::DoubleGrid& grid, double ebv_val){
      for (auto& ebv_axis : grid.getAxis<PhzDataModel::ModelParameter::EBV>()){
        if (abs(ebv_axis-ebv_val)<0.00001){
          return ebv_axis;
        }
      }

      return ebv_val;
  }

  double computeApparentMag(double flux){
    double flux_normalized = flux  / 3631E6 ;
    return -2.5 * std::log10(flux_normalized);
  }

  double computeApparentMagFromModel(
      const PhzDataModel::DoubleGrid::const_iterator& scale_factor,
      std::shared_ptr<PhzDataModel::PhotometryGrid> model_photometry_grid,
      std::shared_ptr<PhzTest::ModelFixingAttribute> model_fixing,
      XYDataset::QualifiedName filter,
      const PhzDataModel::DoubleGrid& scale_factor_grid) {

    auto model_iter = model_photometry_grid->cbegin();
    model_iter.fixAxisByValue < PhzDataModel::ModelParameter::Z> (adjustZAxis(scale_factor_grid, model_fixing->getZ()));
    model_iter.fixAxisByValue < PhzDataModel::ModelParameter::REDDENING_CURVE
        > (model_fixing->getRedCurve());
    model_iter.fixAxisByValue < PhzDataModel::ModelParameter::EBV
        > (adjustEBVAxis(scale_factor_grid, model_fixing->getEbv()));
    model_iter.fixAxisByValue < PhzDataModel::ModelParameter::SED
        > (model_fixing->getSed());

    double flux_normalized = model_iter->find(filter.qualifiedName())->flux
        * (*scale_factor) / 3631E6;
    return -2.5 * std::log10(flux_normalized);
  }

};


MAIN_FOR(MagnitudeForComparison)

