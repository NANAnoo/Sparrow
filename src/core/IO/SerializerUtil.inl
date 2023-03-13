#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <toml++/toml.h>

namespace SPW
{
namespace sut
{
template <typename T> struct toml_traits{
  //      static_assert(false, "toml_traits is not implemented for this type");
};

template <> struct toml_traits<glm::vec3> {
  using array_type = toml::array;

  static inline array_type to_array(const glm::vec3& vec) {
    return array_type{ vec.x, vec.y, vec.z };
  }
};

template <> struct toml_traits<glm::vec4> {
  using array_type = toml::array;

  static inline array_type to_array(const glm::vec4& vec) {
    return array_type{ vec.x, vec.y, vec.z, vec.w };
  }
};

template <> struct toml_traits<glm::quat> {
  using array_type = toml::array;

  static inline array_type to_array(const glm::quat& quat) {
    return array_type{ quat.x, quat.y, quat.z, quat.w };
  }
};

template <typename T>
inline typename toml_traits<T>::array_type to_array(const T& val) {
  return toml_traits<T>::to_array(val);
}
}

}