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

static const std::string LUMINOSITY_FUNCTION_SED_GROUP {"luminosity-function-sed-group-"};
static const std::string LUMINOSITY_FUNCTION_MIN_Z {"luminosity-function-min-z-"};
static const std::string LUMINOSITY_FUNCTION_MAX_Z {"luminosity-function-max-z-"};

static const std::string LUMINOSITY_FUNCTION_CURVE_NAME {"luminosity-function-curve-"};

static const std::string LUMINOSITY_FUNCTION_SCHECHTER_ALPHA {"luminosity-function-schechter-alpha-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_M {"luminosity-function-schechter-m0-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_L {"luminosity-function-schechter-l0-"};
static const std::string LUMINOSITY_FUNCTION_SCHECHTER_PHI {"luminosity-function-schechter-phi0-"};



po::options_description LuminosityFunctionConfiguration::getProgramOptions() {
  po::options_description options {"Luminosity Function options, to be postfixed with the function id."};
  options.add_options()
  ((LUMINOSITY_FUNCTION_SED_GROUP+"<Id>").c_str(), po::value<std::string>(),
      "The SED group the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_MIN_Z+"<Id>").c_str(), po::value<std::string>(),
      "The lower bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_MAX_Z+"<Id>").c_str(), po::value<std::string>(),
      "The upper bound of the redshift range the function is valid for (Mandatory for each Luminosity Function)")
  ((LUMINOSITY_FUNCTION_CURVE_NAME+"<Id>").c_str(), po::value<std::string>(),
      "The sampling of the Curve defining the function (If omitted Schechter options are mandatories)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_ALPHA+"<Id>").c_str(), po::value<std::string>(),
      "The steepness of the Schechter luminosity function (Mandatory if the function-curve is skipped)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_M+"<Id>").c_str(), po::value<std::string>(),
      "The magnitude normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in magnitude)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_L+"<Id>").c_str(), po::value<std::string>(),
      "The flux normalization point of the Schechter luminosity function (Mandatory if the function-curve is skipped and the Luminosity is measured in flux)")
  ((LUMINOSITY_FUNCTION_SCHECHTER_PHI+"<Id>").c_str(), po::value<std::string>(),
      "The normalization value of the Schechter luminosity function (Mandatory if the function-curve is skipped)");

  return merge(LuminosityTypeConfiguration::getProgramOptions())
              (options);
}

PhzLuminosity::LuminosityFunctionSet LuminosityFunctionConfiguration::getLuminosityFunction(){
  auto function_id_list = findWildcardOptions( { LUMINOSITY_FUNCTION_SED_GROUP }, m_options);

  std::map<PhzLuminosity::LuminosityFunctionValidityDomain,std::unique_ptr<MathUtils::Function>> map{};

  for (auto functionId : function_id_list){
    // get the Validity domain
    std::string groupName = getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SED_GROUP+functionId);

    std::stringstream zmin_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_MIN_Z+functionId)};
    double z_min{};
    zmin_stream >> z_min;

    std::stringstream zmax_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_MAX_Z+functionId)};
    double z_max{};
    zmax_stream >> z_max;

    if (z_max<=z_min){
      throw Elements::Exception() << "Conflicting options " << LUMINOSITY_FUNCTION_MIN_Z << functionId
          << " must be smaller than " << LUMINOSITY_FUNCTION_MAX_Z << functionId;
    }

    PhzLuminosity::LuminosityFunctionValidityDomain domain{groupName,z_min,z_max};

    // get the function
    if (m_options.count(LUMINOSITY_FUNCTION_CURVE_NAME+functionId) == 1){
      // Custom function
      std::string curve_name = getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_CURVE_NAME+functionId);
      // TODO get file and read the curve
      auto dataset_identifier = XYDataset::QualifiedName{curve_name};
      std::unique_ptr<XYDataset::FileParser> fp { new XYDataset::AsciiParser{} };
      auto path = getAuxDataDir() / "LuminosityFunctionCurves";
      XYDataset::FileSystemProvider fsp (path.string(), std::move(fp));
      auto dataset_ptr = fsp.getDataset(dataset_identifier);
      auto fct_ptr = MathUtils::interpolate(*(dataset_ptr.get()),MathUtils::InterpolationType::CUBIC_SPLINE);
      map.emplace(std::make_pair(std::move(domain),std::move(fct_ptr)));

    } else {
      //Schechter function
      std::stringstream alpha_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SCHECHTER_ALPHA+functionId)};
      double alpha{};
      alpha_stream >> alpha;

      std::stringstream phi_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SCHECHTER_PHI+functionId)};
      double phi{};
      phi_stream >> phi;

      double m_l{};
      bool inMag = luminosityInMagnitude();
      if (inMag){
        std::stringstream m_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SCHECHTER_M+functionId)};
        m_stream >> m_l;
      } else {
        std::stringstream l_stream {getOptionWithCheck<std::string>(LUMINOSITY_FUNCTION_SCHECHTER_L+functionId)};
        l_stream >> m_l;
      }

      std::unique_ptr<MathUtils::Function> fct_ptr{new PhzLuminosity::SchechterLuminosityFunction{phi,m_l,alpha,inMag}};

      map.emplace(std::make_pair(std::move(domain),std::move(fct_ptr)));
    }



  }

  return PhzLuminosity::LuminosityFunctionSet{std::move(map)};
}


}
}
