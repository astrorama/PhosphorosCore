/**
 * @file PhzDataModel/PhotometryGrid.h
 * @date May 19, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRYGRID_H
#define PHZDATAMODEL_PHOTOMETRYGRID_H

#include "GridContainer/GridContainerToTable.h"
#include "PhzDataModel/PhzModel.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include <memory>
#include <vector>

namespace Euclid {
namespace PhzDataModel {

/**
 * @brief Handle the cells storing the photometry values for the model grid
 * @details
 * To avoid allocating one vector of photometry values per cell, this cell manager
 * does one single allocation of n.cells * n.filters.
 * However, Phosphoros code expects to be able to iterate on the filters for a single cell.
 * To keep compatibility with the existing code, iterating over a grid of photometries yields
 * a proxy object, that can be used in place, and iterated as if it was a
 * regular SourceCatalog::Photometry.
 *
 * Basically it is like a 2D array. The first dimension corresponds to the number of cells, the second to the number
 * of filters.
 */
class PhotometryCellManager {
public:

  /**
   * Proxy class for the photometry values. It provides an API compatible with SourceCatalog::Photometry,
   * hiding the fact that it references a slice of a 2D array.
   * @note A PhotometryProxy is the reference *and* the pointer type for a cell inside a Photometry grid.
   */
  class PhotometryProxy {
  public:
    template <bool Const>
    using PhotometryIterator = typename SourceCatalog::Photometry::PhotometryIterator<Const>;

  protected:
    /**
     * The existing SourceCatalog::Photometry::PhotometryIterator class conveniently accepts
     * an iterator over the filter names, and an iterator over values. Since the PhotometryIterator
     * constructor is protected, this class provides a bypass so PhotometryCellManager can create one.
     * @tparam Const
     */
    template <bool Const>
    class PhotometryIteratorWrapper : public PhotometryIterator<Const> {
    public:
      using typename PhotometryIterator<Const>::values_iter_t;

      PhotometryIteratorWrapper(std::vector<std::string>::const_iterator filters_iter, const values_iter_t& values_iter)
          : SourceCatalog::Photometry::PhotometryIterator<Const>(filters_iter, values_iter) {}
    };

  public:
    typedef PhotometryIterator<true>  const_iterator;
    typedef PhotometryIterator<false> iterator;

    iterator begin() {
      return PhotometryIteratorWrapper<false>(m_parent.m_filter_names.begin(), m_begin);
    }

    iterator end() {
      return PhotometryIteratorWrapper<false>(m_parent.m_filter_names.end(), m_end);
    }

    const_iterator begin() const {
      return PhotometryIteratorWrapper<true>(m_parent.m_filter_names.begin(), m_begin);
    }

    const_iterator end() const {
      return PhotometryIteratorWrapper<true>(m_parent.m_filter_names.end(), m_end);
    }

    std::size_t size() const {
      return m_end - m_begin;
    }

    /**
     * Since PhotometryProxy is the pointer-type, it needs to provide a dereference operator.
     * A dereference operator is recursive, and has to eventually return a pointer.
     * The compiler uses this final pointer to figure out whose method or attribute you are getting.
     */
    PhotometryProxy* operator->() const {
      return const_cast<PhotometryProxy*>(this);
    }

    PhotometryProxy& operator=(const PhotometryProxy& other) {
      std::copy(other.m_begin, other.m_end, m_begin);
      return *this;
    }

    /**
     * For compatibility, the proxy allows being assigned an existing SourceCatalog::Photometry
     * @param photometry
     * @return
     */
    PhotometryProxy& operator=(const SourceCatalog::Photometry& photometry) {
      assert(photometry.size() == size());
      std::copy(photometry.begin(), photometry.end(), m_begin);
      return *this;
    }

    /**
     * For compatibility, transform transparently a proxy into the a SourceCatalog::Photometry.
     * This makes sense when an algorithm takes one cell and does some operations over it. It expects
     * to get a copy and not to modify the original. This is how this can be achieved.
     * @note
     *  This works as long as SourceCatalog::Photometry are sparingly used and short lived.
     *  If it ends being a bottleneck, m_parent.filterNames() could be modified to return directly a shared pointer.
     */
    explicit operator SourceCatalog::Photometry() const {
      return {std::make_shared<std::vector<std::string>>(m_parent.filterNames()),
              std::vector<SourceCatalog::FluxErrorPair>(m_begin, m_end)};
    }

    /**
     * For compatibility
     * @param filter
     * @return
     */
    SourceCatalog::FluxErrorPair* find(const std::string& filter) const {
      auto i = std::find(m_parent.m_filter_names.begin(), m_parent.m_filter_names.end(), filter);
      if (i == m_parent.m_filter_names.end()) {
        throw Elements::Exception() << "Filter " << filter << " not found";
      }
      auto offset = i - m_parent.m_filter_names.begin();
      return &(*(m_begin + offset));
    }

  protected:
    using photometry_iterator = std::vector<SourceCatalog::FluxErrorPair>::iterator;

    /**
     * Constructor.
     * @param parent
     *  PhotometryCellManager that owns the memory.
     * @param begin
     *  Iterator to the first value
     * @param end
     *  Iterator to the one-after-the-last value
     * @note
     *  Going back to the 2D analogy, begin should point to array.begin() + cell_index * n_filters
     */
    PhotometryProxy(const PhotometryCellManager& parent, photometry_iterator begin, photometry_iterator end)
        : m_parent(parent), m_begin(begin), m_end(end){};

    const PhotometryCellManager& m_parent;
    photometry_iterator          m_begin, m_end;

    friend class PhotometryCellManager;
  };

  /**
   * Iterator class to iterate over the cells
   */
  class iterator {
  public:
    iterator(const iterator& other) = default;

    /**
     * PhotometryProxy is the reference-type
     */
    PhotometryProxy operator*() const {
      return {m_parent, m_position, m_position + m_stride};
    }

    /**
     * PhotometryProxy is the pointer type.
     * @details
     *  The operator -> is recursive, and has to eventually return a pointer.
     *  If you call iter->find, for instance, what will happen is that you get
     *  and instance of PhotometryProxy, which is not a pointer. The compiler will look at -> inside PhotometryProxy,
     *  which returns a pointer to itself. This final pointer is usable to access the methods and attributes.
     */
    PhotometryProxy operator->() const {
      return {m_parent, m_position, m_position + m_stride};
    }

    iterator& operator++() {
      m_position += m_stride;
      return *this;
    }

    iterator& operator+=(ssize_t diff) {
      m_position += diff * m_stride;
      return *this;
    }

    ssize_t operator-(const iterator& other) const {
      return (m_position - other.m_position) / m_stride;
    }

    bool operator>(const iterator& other) const {
      return m_position > other.m_position;
    }

    bool operator!=(const iterator& other) const {
      return m_position != other.m_position;
    }

    iterator& operator=(const iterator& other) {
      assert(&m_parent == &other.m_parent);
      assert(m_stride == other.m_stride);
      m_position = other.m_position;
      return *this;
    }

  protected:
    using flux_iterator = std::vector<SourceCatalog::FluxErrorPair>::iterator;

    iterator(const PhotometryCellManager& parent, flux_iterator iter)
        : m_parent(parent), m_position(iter), m_stride(parent.m_filter_names.size()){};

    const PhotometryCellManager& m_parent;
    flux_iterator                m_position;
    ssize_t                      m_stride;

    friend class PhotometryCellManager;
  };

  PhotometryCellManager(size_t size, std::vector<std::string> filter_names);

  PhotometryCellManager(PhotometryCellManager&&) = default;

  PhotometryCellManager(const PhotometryCellManager& other);

  PhotometryCellManager& operator=(const PhotometryCellManager&) = delete;

  size_t size() const {
    return m_size;
  }

  bool empty() const {
    return m_size > 0;
  }

  size_t capacity() const {
    return m_size;
  }

  iterator begin() {
    return iterator(*this, m_data.begin());
  }

  iterator end() {
    return iterator(*this, m_data.end());
  }

  PhotometryProxy operator[](size_t i) {
    auto _begin = m_data.begin() + i * m_filter_names.size();
    auto _end   = _begin + m_filter_names.size();
    return {*this, _begin, _end};
  }

  const std::vector<std::string>& filterNames() const {
    return m_filter_names;
  }

  const std::vector<std::string>& getConstructorParameters() const {
    return m_filter_names;
  }

private:
  size_t                                    m_size;
  std::vector<std::string>                  m_filter_names;
  std::vector<SourceCatalog::FluxErrorPair> m_data;
  friend class PhotometryCellManager::iterator;
};

typedef PhzGrid<PhotometryCellManager> PhotometryGrid;

}  // end of namespace PhzDataModel

namespace GridContainer {

template <>
struct GridCellToTable<PhzDataModel::PhotometryCellManager::PhotometryProxy> {

  /**
   * Get the column descriptions of the values of the cell. The element passed will be one
   * reference cell from the grid (i.e. the first one)
   * @param c
   *    A cell instance
   * @param columns
   *    The column description(s) for the cell type. New columns must be *appended*
   */
  static void addColumnDescriptions(const PhzDataModel::PhotometryCellManager::PhotometryProxy&,
                                    std::vector<Table::ColumnDescription>&) {
    assert(false);
  }

  /**
   * Add the cell values into the row
   * @param c
   *    A cell instance to be serialized
   * @param row
   *    Destination row. New cells must be *appended* on the same order as the column descriptions.
   */
  static void addCells(const PhzDataModel::PhotometryCellManager::PhotometryProxy&, std::vector<Table::Row::cell_type>&) {
    assert(false);
  }
};

/**
 * @struct Euclid::GridContainer::GridCellManagerTraits<PhotometryCellManager>
 * @brief Specialization of the GridCellManagerTraits template.
 *
 * @details
 * We define the GridCellManagerTraits for a vector of Photometries to redefine the
 * factory method because the Photometry does not have default constructor
 */
template <>
struct GridCellManagerTraits<PhzDataModel::PhotometryCellManager> {
  typedef std::vector<std::string>                             constructor_parameters;
  typedef PhzDataModel::PhotometryCellManager::PhotometryProxy data_type;
  typedef PhzDataModel::PhotometryCellManager::PhotometryProxy pointer_type;
  typedef PhzDataModel::PhotometryCellManager::PhotometryProxy reference_type;

  typedef typename PhzDataModel::PhotometryCellManager::iterator iterator;

  /**
   * @brief Factory to build PhotometryCellManager which has no default constructor
   *
   * @param size
   * The size of the photometry storage
   *
   * @return
   * A unique_ptr on the new PhotometryCellManager.
   *
   */
  static std::unique_ptr<PhzDataModel::PhotometryCellManager> factory(size_t size, std::vector<std::string> filter_names);

  static std::unique_ptr<PhzDataModel::PhotometryCellManager> factory(size_t                                size,
                                                                      std::vector<XYDataset::QualifiedName> filter_names);

  /**
   * @brief Initialize from another PhotometryCellManager
   */
  static std::unique_ptr<PhzDataModel::PhotometryCellManager> factory(size_t                                     size,
                                                                      const PhzDataModel::PhotometryCellManager& other);

  /**
   * @brief return the size of the PhotometryCellManager
   */
  static size_t size(const PhzDataModel::PhotometryCellManager& manager);

  /**
   * @brief static iterator on the PhotometryCellManager
   */
  static iterator begin(PhzDataModel::PhotometryCellManager& manager);

  /**
   * @brief static iterator on the PhotometryCellManager
   */
  static iterator end(PhzDataModel::PhotometryCellManager& manager);

  static const bool enable_boost_serialize = true;
};  // end of GridCellManagerTraits

}  // end of namespace GridContainer
}  // end of namespace Euclid

// Here we include the serialization of the photometry grid. This is done here
// to avoid having the default grid serialization applied to the PhotometryGrid
// (which would happen if the user would forget to include this file)
#include "PhzDataModel/serialization/PhotometryGrid.h"

#endif /* PHZDATAMODEL_PHOTOMETRYGRID_H */
