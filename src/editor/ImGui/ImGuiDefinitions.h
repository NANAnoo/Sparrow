/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <variant>
#include <memory>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SPW
{

using ComponentCallback = std::function<void()>;

enum class UIComponentType {
  Panel,
  Dockspace,
  MenuBar,
  ObjectPanel,
  HierarchyPanel,
  InspectorPanel
};

enum class GameOjbectComponentType {
  Transform,
  Camera,
  Light,
  Renderer,
  // ... Add more component types.
};

template <GameOjbectComponentType T> struct ComponentData {};

template <> struct ComponentData<GameOjbectComponentType::Transform> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  ComponentData() = default;
  ComponentData(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
      : position(std::move(position)), rotation(std::move(rotation)),
        scale(std::move(scale)) {}
};

template <> struct ComponentData<GameOjbectComponentType::Light> {
  glm::vec3 position;
  // TODO Add more attributes, and move position to transform
  ComponentData() = default;
  ComponentData(glm::vec3 position) : position(std::move(position)) {}
};

template <> struct ComponentData<GameOjbectComponentType::Camera> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

template <> struct ComponentData<GameOjbectComponentType::Renderer> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

// Continue with other component types

using ComponentVariant =
    std::variant<ComponentData<GameOjbectComponentType::Transform>,
                 ComponentData<GameOjbectComponentType::Camera>,
                 ComponentData<GameOjbectComponentType::Light>,
                 ComponentData<GameOjbectComponentType::Renderer>
                 // ... Add more component types.
                 >;

struct GameOjbectComponent {
  GameOjbectComponentType type;
  ComponentVariant
      data; // which is point to the data of the component of the game object.
  ComponentCallback callback;
};

struct GameObject {
  std::string name;
  std::vector<GameOjbectComponent> components;
  // Add more object properties if needed.

  GameObject() = default;
  GameObject(const std::string &name,
             const std::vector<GameOjbectComponent> &components)
      : name(name), components(components) {}
};

}