/**
 * @file LuminosityFunctionConfiguration.cpp
 * @date August 20, 2015
 * @author dubathf
 */



#include <string>
#include "PhzConfiguration/ProgramOptionsHelper.h"

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "MathUtils/interpolation/interpolation.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"
#include "PhzConfiguration/LuminosityFunctionConfiguration.h"
#include "PhzLuminosity/SchechterLuminosityFunction.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static const std::string LUMINOSITY_FUNCTION_SED_GROUP {"luminosity-function-sed-group"};
static const std::string LUMINOSITY_FUNCTION_MIN_Z {"luminosity-function-min-z"};
static const std::string LUMINOSITY_FUNCTION_MAX_Z {"luminosity-function-max-z"};

static const std::string LUMINOSITY_FUNCTION_CURVE_NAME {"luminosity-function-curve"};

static const std::string LUMINOSITY_FUNCTION_SCHECHTER_ALPHA {"luminosity-function-schechter-alpha"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_M {"luminosity-function-schechter-m0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_L {"luminosity-function-schechter-l0"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_PHI {"luminosity-function-schechter-phi0"};



po::options_description LuminosityFunctionConfiguration::getProgramOptions() {
  po::options_description options {"Luminosity Function options, to be postfixed with the function id."};
  options.add_options()
  ((LUMINOSITY_FUNCTION_SED_GROUP+"-*").c_str(), po::value<std::string>(),
      "The SED group the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_MIN_Z+"-*").c_str(), po::value<double>(),
      "The lower bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_MAX_Z+"-*").c_str(), po::value<double>(),
      "The upper bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_CURVE_NAME+"-*").c_str(), po::value<std::string>(),
      "The sampling of the Curve defining the function (If omitted Schechter options are mandatories)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_ALPHA+"-*").c_str(), po::value<double>(),
      "The steepness of the Schechter luminosity function (Mandatory if the function-curve is skipped)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_M+"-*").c_str(), po::value<double>(),
      "The magnitude normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in magnitude)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_L+"-*").c_str(), po::value<double>(),
      "The flux normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in flux)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_PHI+"-*").c_str(), po::value<double>(),
      "The normalization value of the Schechter luminosity function (Mandatory if the function-curve is skipped)");

  return merge(LuminosityTypeConfiguration::getProgramOptions())
              (options);
}

PhzLuminosity::LuminosityFunctionSet LuminosityFunctionConfiguration::getLuminosityFunction(){
  auto function_id_list = findWildcardOptions( { LUMINOSITY_FUNCTION_SED_GROUP }, m_options);

  std::vector<std::pair<PhzLuminosity::LuminosityFunctionValidityDomain,
                               std::unique_ptr<MathUtils::Function>>> vector{};

  for (auto functionId : function_id_list){
    // get the Validity domain
    std::string groupName = getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SED_GROUP+"-"+functionId);


    double z_min=getOptionWithCheck<double>(LUMINOSITY_FUNCTION_MIN_Z+"-"+functionId);
    double z_max=getOptionWithCheck<double>(LUMINOSITY_FUNCTION_MAX_Z+"-"+functionId);



    if (z_max<=z_min){
      throw Elements::Exception() << "Conflicting options " << LUMINOSITY_FUNCTION_MIN_Z<< "-"<< functionId
          << " must be smaller than " << LUMINOSITY_FUNCTION_MAX_Z<< "-"<< functionId;
    }

    PhzLuminosity::LuminosityFunctionValidityDomain domain{groupName,z_min,z_max};

    // get the function
    if (m_options.count(LUMINOSITY_FUNCTION_CURVE_NAME+"-"+functionId) == 1){
      // Custom function
      std::string curve_name = getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_CURVE_NAME+"-"+functionId);
      auto dataset_identifier = XYDataset::QualifiedName{curve_name};
      std::unique_ptr<XYDataset::FileParser> fp { new XYDataset::AsciiParser{} };
      auto path = getAuxDataDir() / "LuminosityFunctionCurves";
      XYDataset::FileSystemProvider fsp (path.string(), std::move(fp));
      auto dataset_ptr = fsp.getDataset(dataset_identifier);
      auto fct_ptr = MathUtils::interpolate(*(dataset_ptr.get()),MathUtils::InterpolationType::CUBIC_SPLINE);
      vector.push_back(std::make_pair(std::move(domain),std::move(fct_ptr)));

    } else {
      //Schechter function

      double alpha{getOptionWithCheck<double>(LUMINOSITY_FUNCTION_SCHECHTER_ALPHA+"-"+functionId)};
      double phi{getOptionWithCheck<double>(LUMINOSITY_FUNCTION_SCHECHTER_PHI+"-"+functionId)};


      double m_l{};
      bool inMag = luminosityInMagnitude();
      if (inMag){
        m_l=getOptionWithCheck<double>(LUMINOSITY_FUNCTION_SCHECHTER_M+"-"+functionId);
      } else {
        m_l=getOptionWithCheck<double>(LUMINOSITY_FUNCTION_SCHECHTER_L+"-"+functionId);
      }

      std::unique_ptr<MathUtils::Function> fct_ptr{new PhzLuminosity::SchechterLuminosityFunction{phi,m_l,alpha,inMag}};

      vector.push_back(std::make_pair(std::move(domain),std::move(fct_ptr)));
    }



  }

  return PhzLuminosity::LuminosityFunctionSet{std::move(vector)};
}


}
}
