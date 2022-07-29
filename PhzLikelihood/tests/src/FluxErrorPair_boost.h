/**
 * @file FluxErrorPair_boost.h
 * @date December 21, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef FLUXERRORPAIR_BOOST_H
#define FLUXERRORPAIR_BOOST_H

#include <SourceCatalog/SourceAttributes/Photometry.h>

inline std::ostream& operator<<(std::ostream& os, const Euclid::SourceCatalog::FluxErrorPair& item) {
  return os << '(' << item.flux << ',' << item.error << ')';
}

namespace boost {

namespace test_tools {

#if BOOST_VERSION >= 106100
namespace tt_detail {
#endif

template <>
struct print_log_value<Euclid::SourceCatalog::FluxErrorPair> {
  void operator()(std::ostream& os, const Euclid::SourceCatalog::FluxErrorPair& item) const {
    ::operator<<(os, item);
  }
};

#if BOOST_VERSION >= 106100
}  // namespace tt_detail
#endif

}  // namespace test_tools

}  // namespace boost

#endif /* FLUXERRORPAIR_BOOST_H */
