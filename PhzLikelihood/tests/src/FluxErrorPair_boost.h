/** 
 * @file FluxErrorPair_boost.h
 * @date December 21, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef FLUXERRORPAIR_BOOST_H
#define	FLUXERRORPAIR_BOOST_H

namespace std {

inline ostream& operator<<(ostream& wrapped, const Euclid::SourceCatalog::FluxErrorPair& item) {
  return wrapped << '(' << item.flux << ',' << item.error << ')';
}

} // namespace std



namespace boost {

/// Enables boost to print objects of type FluxErrorPair
inline wrap_stringstream& operator<<(wrap_stringstream& wrapped, const Euclid::SourceCatalog::FluxErrorPair& item) {
  return wrapped << '(' << item.flux << ',' << item.error << ')';
}

} // namespace boost

#endif	/* FLUXERRORPAIR_BOOST_H */

