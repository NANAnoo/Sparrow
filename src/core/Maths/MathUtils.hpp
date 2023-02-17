/*
 * Created by elecfrog@du
 * 2023/02/16
 * Define Some Math Utils to make Life Easier 
 */

#pragma once

// Include GLM for Math computing
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


inline glm::vec3 ToGLMVec3(const float _v[3])
{
  return glm::vec3(_v[0], _v[1], _v[2]);
}

inline float* ToFloat3(const glm::vec3& _v)
{
  float r[3] = { _v[0], _v[1], _v[2] };

  return r;
}

inline float* ToFloat3(glm::vec3&& _v)
{
  float r[3] = { _v[0], _v[1], _v[2] };

  return r;
}