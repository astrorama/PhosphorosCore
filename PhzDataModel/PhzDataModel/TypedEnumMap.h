/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */

/* 
 * @file PhzDataModel/TypedEnumMap.h
 * @author nikoapos
 */

#ifndef _PHZDATAMODEL_TYPEDENUMMAP_H
#define _PHZDATAMODEL_TYPEDENUMMAP_H

#include <map>
#include <memory>
#include "ElementsKernel/Exception.h"

namespace Euclid {
namespace PhzDataModel {

template <typename TypedEnum, TypedEnum T, typename=void>
struct TypedEnumTraits {
  // The T != T is used because the static assert must use a template parameter,
  // otherwise it will be executed even without template instantiation.
  static_assert(T != T,
      "READ THE MESSAGE BELLOW!!!\n\n"
      "If you get this message you added a new entry to a typed\n"
      "enumeration without specifying the traits class which handles\n"
      "it. Read the documentation in PhzDataModel/TypedEnumMap.h\n"
      "file to understand the steps for fixing your problem.\n");
};

template <typename TypedEnum>
class TypedEnumMap final {
  
public:
  
  template <TypedEnum T, typename... Args>
  typename TypedEnumTraits<TypedEnum, T>::type& set(Args&&... args) {
    if (object_map.find(T) != object_map.end()) {
      throw Elements::Exception() << "TypedEnumMap instance already contains the object "
                    << "of enumeration " << typeid(TypedEnum).name() << " with index " 
                    << static_cast<typename std::underlying_type<TypedEnum>::type>(T);
    }
    object_map[T] = std::make_shared<typename TypedEnumTraits<TypedEnum, T>::type>(std::forward<Args>(args)...);
    return get<T>();
  }
  
  template <TypedEnum T>
  const typename TypedEnumTraits<TypedEnum, T>::type& get() const {
    auto result_iter = object_map.find(T);
    if (result_iter == object_map.end()) {
      throw Elements::Exception() << "TypedEnumMap instance does not contain the result "
                    << "of enumeration " << typeid(TypedEnum).name()<< " with index " 
                    << static_cast<typename std::underlying_type<TypedEnum>::type>(T);
    }
    return *static_cast<typename TypedEnumTraits<TypedEnum, T>::type*>(result_iter->second.get());
  }
  
  template <TypedEnum T>
  typename TypedEnumTraits<TypedEnum, T>::type& get() {
    auto result_iter = object_map.find(T);
    if (result_iter == object_map.end()) {
      throw Elements::Exception() << "TypedEnumMap instance does not contain the result "
                    << "of enumeration " << typeid(TypedEnum).name()<< " with index " 
                    << static_cast<typename std::underlying_type<TypedEnum>::type>(T);
    }
    return *static_cast<typename TypedEnumTraits<TypedEnum, T>::type*>(result_iter->second.get());
  }
  
  template <TypedEnum T>
  bool contains() const {
    return object_map.count(T) > 0;
  }

private:
  
  // Note to developers:
  // We use shared_ptr, to allow both copying and moving of the TypeEnumMaps.
  // This makes copying a shallow copy, which means that the underlying objects
  // are being shared.
  std::map<TypedEnum, std::shared_ptr<void>> object_map {};
  
};

} /* namespace PhzDataModel */
} /* namespace Euclid */

#endif /* _PHZDATAMODEL_TYPEDENUMMAP_H */

