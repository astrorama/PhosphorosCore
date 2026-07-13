/**
 * @file PhzDataModel/PhotometryGrid.cpp
 * @date Sep 23, 2014
 * @author Florian Dubath
 */

#include "PhzDataModel/PhotometryGrid.h"
#include "AlexandriaKernel/memory_tools.h"

namespace Euclid {

namespace PhzDataModel {

PhotometryCellManager::PhotometryCellManager(size_t size, std::vector<std::string> filter_names, std::vector<std::string> scaling_filter_names)
    : m_size(size)
    , m_filter_names(std::move(filter_names))
    , m_data(size * m_filter_names.size(), SourceCatalog::FluxErrorPair(0, 0))
    , m_scaling_filter_names(std::move(scaling_filter_names))
    , m_differentiel_scaling(size * m_scaling_filter_names.size(), 1.0) {}

PhotometryCellManager::PhotometryCellManager(const PhotometryCellManager& other)
    : m_size(other.m_size)
    , m_filter_names(other.m_filter_names)
    , m_data(other.m_data)
    , m_scaling_filter_names(other.m_scaling_filter_names)
    , m_differentiel_scaling(other.m_differentiel_scaling)  {}

}  // namespace PhzDataModel

namespace GridContainer {

std::unique_ptr<PhzDataModel::PhotometryCellManager>
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::factory(size_t                   size,
                                                                    std::vector<std::string> filter_names,
                                                                    std::vector<std::string> scaling_filter_names) {
  return make_unique<PhzDataModel::PhotometryCellManager>(size, filter_names, scaling_filter_names);
}

std::unique_ptr<PhzDataModel::PhotometryCellManager>
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::factory(size_t size, 
                                                                    std::vector<XYDataset::QualifiedName> filter_names,
                                                                    std::vector<XYDataset::QualifiedName> scaling_filter_names) {
  std::vector<std::string> str_filter_names(filter_names.size());
  std::transform(filter_names.begin(), filter_names.end(), str_filter_names.begin(),
                 [](const XYDataset::QualifiedName& q) {
                   return q.qualifiedName();
                 });
  std::vector<std::string> str_scaling_filter_names(scaling_filter_names.size());
  std::transform(scaling_filter_names.begin(), scaling_filter_names.end(), str_scaling_filter_names.begin(),
                 [](const XYDataset::QualifiedName& q) {
                   return q.qualifiedName();
                 });
  return factory(size, str_filter_names, str_scaling_filter_names);
}

std::unique_ptr<PhzDataModel::PhotometryCellManager>
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::factory(size_t                                     size,
                                                                    const PhzDataModel::PhotometryCellManager& other) {
  assert(size == other.size());
  return make_unique<PhzDataModel::PhotometryCellManager>(other);
}

size_t
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::size(const PhzDataModel::PhotometryCellManager& manager) {
  return manager.size();
}

PhzDataModel::PhotometryCellManager::iterator
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::begin(PhzDataModel::PhotometryCellManager& manager) {
  return manager.begin();
}

PhzDataModel::PhotometryCellManager::iterator
GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::end(PhzDataModel::PhotometryCellManager& manager) {
  return manager.end();
}

}  // namespace GridContainer
}  // namespace Euclid
