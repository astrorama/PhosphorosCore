/**
 * @file src/lib/SedClassifier.cpp
 * @date 2019-03-15
 * @author Florian dubath
 */

#include "PhzNzPrior/SedClassifier.h"

#include <iterator>
#include <cmath>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "XYDataset/XYDataset.h"
#include "PhzDataModel/QualifiedNameGroupManager.h"

#include "MathUtils/interpolation/interpolation.h"

#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzDataModel/FilterInfo.h"



namespace Euclid {
namespace PhzNzPrior {

static Elements::Logging logger = Elements::Logging::getLogger("SedClassifier");

double computeB_I(const PhzDataModel::FilterInfo& filter_b,
                  const PhzDataModel::FilterInfo& filter_i,
                  const XYDataset::XYDataset& sed) {
  auto apply_filter_funct = PhzModeling::ApplyFilterFunctor{};
  auto integrate_dataset_funct = PhzModeling::IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR};

  auto filterd_b = apply_filter_funct(sed, filter_b.getRange(), filter_b.getFilter());
  auto flux_b = integrate_dataset_funct(filterd_b, filter_b.getRange())/filter_b.getNormalization();
  if (flux_b <= 0) {
    throw Elements::Exception() << "PhzNzPrior::SedClassifier: A SED has 0 flux in provided B filter ";
  }

  auto filterd_i = apply_filter_funct(sed, filter_i.getRange(), filter_i.getFilter());
  auto flux_i = integrate_dataset_funct(filterd_i, filter_i.getRange()) / filter_i.getNormalization();
  if (flux_i <= 0) {
    throw Elements::Exception() << "PhzNzPrior::SedClassifier: A SED has 0 flux in provided I filter ";
  }

  double mag_b = -2.5*std::log10(flux_b);
  double mag_i = -2.5*std::log10(flux_i);

  return mag_b - mag_i;
}

SedClassifier::SedClassifier(std::shared_ptr<XYDataset::XYDatasetProvider>& filter_provider,
    std::shared_ptr<XYDataset::XYDatasetProvider>& sed_provider, double bi_break_1, double bi_break_2):
                          m_filter_provider{filter_provider}, m_sed_provider{sed_provider},
                          m_break_1{bi_break_1}, m_break_2{bi_break_2} {
  if (m_break_1 >= m_break_2){
    throw Elements::Exception() << "PhzNzPrior::SedClassifier: bi_break_1 (" << m_break_1
                                <<") must be smaller than bi_break_2(" << m_break_2 <<")";
  }
}


PhzDataModel::QualifiedNameGroupManager SedClassifier::operator()(const XYDataset::QualifiedName& b_filter,
                                                     const XYDataset::QualifiedName& i_filter,
                                                     const std::vector<XYDataset::QualifiedName> & seds) const {

    logger.info()<< "Classifying" << seds.size() <<" SEDs between T1 (E/So), T2 (Sp) and T3 (Irr) categories";
    auto b_ptr = m_filter_provider->getDataset(b_filter);
    if (!b_ptr) {
      throw Elements::Exception() << "PhzNzPrior::SedClassifier: Failed to find dataset: " << b_filter.qualifiedName();
    }
    auto filter_info_b = PhzModeling::BuildFilterInfoFunctor {}(*b_ptr);

    auto i_ptr = m_filter_provider->getDataset(i_filter);
    if (!i_ptr) {
      throw Elements::Exception() << "PhzNzPrior::SedClassifier: Failed to find dataset: " << i_filter.qualifiedName();
    }
    auto filter_info_i = PhzModeling::BuildFilterInfoFunctor {}(*i_ptr);

    std::set<XYDataset::QualifiedName> T1_sed_set{};
    std::set<XYDataset::QualifiedName> T2_sed_set{};
    std::set<XYDataset::QualifiedName> T3_sed_set{};

    auto sed_iter = seds.begin();
    while (sed_iter != seds.end()) {
      auto sed_ptr = m_sed_provider->getDataset(*sed_iter);
      if (!sed_ptr) {
       throw Elements::Exception() << "PhzNzPrior::SedClassifier: Failed to find dataset: " << sed_iter->qualifiedName();
      }

      double B_I_rest = computeB_I(filter_info_b, filter_info_i, *sed_ptr);

      if (B_I_rest > m_break_2) {
        T1_sed_set.insert(*sed_iter);
      } else if (B_I_rest > m_break_1) {
        T2_sed_set.insert(*sed_iter);
      } else {
        T3_sed_set.insert(*sed_iter);
      }

      ++sed_iter;
    }

    auto groupList = std::map<std::string, std::set<XYDataset::QualifiedName>>();

    groupList.insert(std::make_pair("T1", T1_sed_set));
    groupList.insert(std::make_pair("T2", T2_sed_set));
    groupList.insert(std::make_pair("T3", T3_sed_set));

    return PhzDataModel::QualifiedNameGroupManager(groupList);
}

}
}
