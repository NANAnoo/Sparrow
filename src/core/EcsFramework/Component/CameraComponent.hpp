//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"

namespace SPW {
//  camera type, used for building ProjectionMatrix
// default direction is (0, 0, -1)
enum CameraType {
  PerspectiveType, // Perspective camera
  OrthoType, // Orthographic camera
  UIOrthoType // only used for UI, still an ortho camera
};
class CameraComponent : public ComponentI {
public:
  CameraComponent() = delete;
  explicit CameraComponent(CameraType type) : cameraType(type) {}
  // PerspectiveType
  float fov = 1.0;
  float aspect = 1.0;
  float near = 0.1;
  float far = 10;

  // ortho type
  float left = 0;
  float right = 0;
  float bottom = 1;
  float top = 1;
  [[nodiscard]] CameraType getType() const { return cameraType;}

  // init from lua
  void initFromLua(const sol::table &value) final {
    if (value["fov"].valid()) {
      fov = value["fov"];
    }
    if (value["aspect"].valid()) {
      aspect = value["aspect"];
    }
    if (value["near"].valid()) {
        near = value["near"];
    }
    if (value["far"].valid()) {
        far = value["far"];
    }
    if (value["left"].valid()) {
      left = value["left"];
    }
    if (value["right"].valid()) {
      right = value["right"];
    }
    if (value["bottom"].valid()) {
      bottom = value["bottom"];
    }
    if (value["top"].valid()) {
      top = value["top"];
    }
    if (value["is_main"].valid()) {
      whetherMainCam = value["is_main"];
    }
  }

  // udpate from lua
  void update(const std::string &key, const sol::table &value) final {
    if (key == "fov" && value["value"].valid()) {
      fov = value["value"];
    }
    if (key == "aspect" && value["value"].valid()) {
      aspect = value["value"];
    }
    if (key == "near" && value["value"].valid()) {
        near = value["value"];
    }
    if (key == "far" && value["value"].valid()) {
        far = value["value"];
    }
    if (key == "left" && value["value"].valid()) {
      left = value["value"];
    }
    if (key == "right" && value["value"].valid()) {
      right = value["value"];
    }
    if (key == "bottom" && value["value"].valid()) {
      bottom = value["value"];
    }
    if (key == "top" && value["value"].valid()) {
      top = value["value"];
    }
    if (key == "is_main" && value["value"].valid()) {
      whetherMainCam = value["value"];
    }
  }

  // getLuaValue
  virtual sol::object getLuaValue(const sol::table &value, const std::string &key) final {
    if (key == "fov") {
      return sol::make_object(value.lua_state(), fov);
    }
    if (key == "aspect") {
      return sol::make_object(value.lua_state(), aspect);
    }
    if (key == "near") {
      return sol::make_object(value.lua_state(), near);
    }
    if (key == "far") {
      return sol::make_object(value.lua_state(), far);
    }
    if (key == "left") {
      return sol::make_object(value.lua_state(), left);
    }
    if (key == "right") {
      return sol::make_object(value.lua_state(), right);
    }
    if (key == "bottom") {
      return sol::make_object(value.lua_state(), bottom);
    }
    if (key == "top") {
      return sol::make_object(value.lua_state(), top);
    }
    if (key == "is_main") {
      return sol::make_object(value.lua_state(), whetherMainCam);
    }
    return sol::nil;
  }

  //whether main
  bool whetherMainCam = false;
private:
  CameraType cameraType = PerspectiveType;
};
}