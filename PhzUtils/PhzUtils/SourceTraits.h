/*
 * SourceTraits.h
 *
 *  Created on: Mar 29, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef PHZUTILS_PHZUTILS_SOURCETRAITS_H_
#define PHZUTILS_PHZUTILS_SOURCETRAITS_H_

#include <type_traits>

namespace Euclid {
namespace PhzUtils {

template <typename SourceIter>
struct SourceIterTraits {
    using id_type = decltype(std::declval<typename SourceIter::value_type>().getId());
};

template <typename Source>
struct SourceTraits {
    using id_type = decltype(std::declval<Source>().getId());
};

}
}

#endif //PHZUTILS_PHZUTILS_SOURCETRAITS_H_
