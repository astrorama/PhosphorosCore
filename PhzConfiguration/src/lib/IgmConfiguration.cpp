/** 
 * @file IgmConfiguration.cpp
 * @date April 29, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"
#include "PhzModeling/MadauNewIgmFunctor.h"
#include "PhzModeling/MeiksinIgmFunctor.h"
#include "PhzModeling/InoueIgmFunctor.h"
#include "PhzConfiguration/IgmConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description IgmConfiguration::getProgramOptions() {
  po::options_description options {"IGM absorption options"};
  options.add_options()
  ("igm-absorption-type", po::value<std::string>(),
        "The type of IGM absorption to apply (one of OFF, MADAU, MADAU_NEW, "
        "MADAU_NEW_FAST, MADAU_NEW+METALS, MEIKSIN, MEIKSIN+OTS, "
        "MEIKSIN+OTS+LLS, MEIKSIN+OTS+LLS+BLUE_FIX, INOUE)");
  return options;
}

PhzModeling::PhotometryGridCreator::IgmAbsorptionFunction IgmConfiguration::getIgmAbsorptionFunction() {
  auto type = getIgmAbsorptionType();
  if (m_options["igm-absorption-type"].empty()) { 
    throw Elements::Exception() << "Missing mandatory parameter igm-absorption-type";
  }
  if (type == "OFF") {
    return PhzModeling::NoIgmFunctor{};
  }
  if (type == "MADAU") {
    return PhzModeling::MadauIgmFunctor{};
  }
  if (type == "MADAU_NEW") {
    return PhzModeling::MadauNewIgmFunctor{false, false};
  }
  if (type == "MADAU_NEW_FAST") {
    return PhzModeling::MadauNewIgmFunctor{true, false};
  }
  if (type == "MADAU_NEW+METALS") {
    return PhzModeling::MadauNewIgmFunctor{false, true};
  }
  if (type == "MEIKSIN") {
    return PhzModeling::MeiksinIgmFunctor{false, false, false};
  }
  if (type == "MEIKSIN+OTS") {
    return PhzModeling::MeiksinIgmFunctor{true, false, false};
  }
  if (type == "MEIKSIN+OTS+LLS") {
    return PhzModeling::MeiksinIgmFunctor{true, true, false};
  }
  if (type == "MEIKSIN+OTS+LLS+BLUE_FIX") {
    return PhzModeling::MeiksinIgmFunctor{true, true, true};
  }
  if (type == "INOUE") {
    return PhzModeling::InoueIgmFunctor{};
  }
  throw Elements::Exception() << "Unknown IGM absorption type \"" << type << "\"";
}

std::string IgmConfiguration::getIgmAbsorptionType() {
  if (!m_options["igm-absorption-type"].empty()) { 
    return m_options["igm-absorption-type"].as<std::string>();
  }
  throw Elements::Exception() << "Missing mandatory parameter igm-absorption-type";
}

}
}