/*
 * CheckLuminosityParameter.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: fdubath
 */


#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"
#include "CheckLuminosityParameter.h"



namespace Euclid {
namespace PhzConfiguration {


static Elements::Logging logger = Elements::Logging::getLogger("CheckLuminosityParameter");

bool CheckLuminosityParameter::checkSedGroupCompletness(const PhzDataModel::PhotometryGridInfo& modelGridInfo,
                    const PhzDataModel::QualifiedNameGroupManager& sedGroupManager) {

  auto grid_seds = getCompleteList<XYDataset::QualifiedName,PhzDataModel::ModelParameter::SED>(modelGridInfo);

  // check that the cardinality match
  std::size_t group_size = 0;
  for (auto& pair : sedGroupManager.getManagedGroups()) {
    group_size += pair.second.size();
  }

  if (group_size != grid_seds.size()){
    logger.info() << "SED lists have not the same size (SEDs in SED groups) " << group_size
                  << " != (SEDs in the grid) " << grid_seds.size();
    return false;
  }

  // check that all grid SED are known by the manager
  try{
    for (auto& sed : grid_seds){
      sedGroupManager.findGroupContaining(sed);
    }
  }catch(const Elements::Exception&) {
    return false;
  }

  return true;
}

bool CheckLuminosityParameter::checkLuminosityModelGrid(const PhzDataModel::PhotometryGridInfo& modelGridInfo,
                const PhzDataModel::PhotometryGrid& luminosityGrid,bool withReddening){

  auto luminosity_z = luminosityGrid.getAxis<PhzDataModel::ModelParameter::Z>();
  bool found=false;
  for (double z:luminosity_z){
    if (z==0.){
      found=true;
      break;
    }
  }
  if (!found) return false;

  if (!checkAxis<XYDataset::QualifiedName,PhzDataModel::ModelParameter::SED>(modelGridInfo,luminosityGrid)){
    return false;
  }

  if (!checkAxis<XYDataset::QualifiedName,PhzDataModel::ModelParameter::REDDENING_CURVE>(modelGridInfo,luminosityGrid)){
    return false;
  }

  if (withReddening){
    if (!checkAxis<double,PhzDataModel::ModelParameter::EBV>(modelGridInfo,luminosityGrid)){
      return false;
    }
  } else {
    auto luminosity_ebv = luminosityGrid.getAxis<PhzDataModel::ModelParameter::EBV>();
     found=false;
     for (double ebv:luminosity_ebv){
       if (ebv==0.){
         found=true;
         break;
       }
     }
     if (!found) return false;
  }

  return true;
}



}
}
