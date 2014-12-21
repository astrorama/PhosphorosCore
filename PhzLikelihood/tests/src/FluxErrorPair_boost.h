/** 
 * @file FluxErrorPair_boost.h
 * @date December 21, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef FLUXERRORPAIR_BOOST_H
#define	FLUXERRORPAIR_BOOST_H

namespace boost {

/// Enables boost to print objects of type FluxErrorPair
inline wrap_stringstream& operator<<(wrap_stringstream& wrapped, const Euclid::SourceCatalog::FluxErrorPair& item) {
  return wrapped << '(' << item.flux << ',' << item.error << ')';
}

/// Enables boost to compare objects of type FluxErrorPair
inline bool operator!=(const Euclid::SourceCatalog::FluxErrorPair& left, const Euclid::SourceCatalog::FluxErrorPair& right) {
  return left.flux != right.flux || left.error != right.error;
}

}

#endif	/* FLUXERRORPAIR_BOOST_H */

