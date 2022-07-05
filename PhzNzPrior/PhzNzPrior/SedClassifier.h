/**
 * @file PhzNzPrior/SedClassifier.h
 * @date 2019-03-14
 * @author Florian dubath
 */

#ifndef PHZ_NZ_PRIOR_SEDCLASSIFIER_H_
#define PHZ_NZ_PRIOR_SEDCLASSIFIER_H_

#include "PhzDataModel/QualifiedNameGroupManager.h"
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDatasetProvider.h"
#include <memory>
#include <vector>

namespace Euclid {
namespace PhzNzPrior {
/**
 * @class Euclid::PhzNzPrior::SedClassifier
 *
 */
class SedClassifier {
public:
  SedClassifier(std::shared_ptr<XYDataset::XYDatasetProvider> filter_provider,
                std::shared_ptr<XYDataset::XYDatasetProvider> sed_provider, double bi_break_1 = 0.945,
                double bi_break_2 = 1.285);

  /**
   * @brief destructor
   */
  virtual ~SedClassifier() = default;

  PhzDataModel::QualifiedNameGroupManager operator()(const XYDataset::QualifiedName&              b_filter,
                                                     const XYDataset::QualifiedName&              i_filter,
                                                     const std::vector<XYDataset::QualifiedName>& seds) const;

private:
  std::shared_ptr<XYDataset::XYDatasetProvider> m_filter_provider;
  std::shared_ptr<XYDataset::XYDatasetProvider> m_sed_provider;
  double                                        m_break_1;
  double                                        m_break_2;
};
}  // namespace PhzNzPrior
}  // namespace Euclid

#endif /* PHZ_NZ_PRIOR_SEDCLASSIFIER_H_ */
