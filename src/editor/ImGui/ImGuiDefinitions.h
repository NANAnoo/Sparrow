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

enum class EntityComponentType {
  Transform,
  Camera,
  Light,
  Renderer,
  // ... Add more component types.
};

template <EntityComponentType T> struct ComponentData {};

template <> struct ComponentData<EntityComponentType::Transform> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  ComponentData() = default;
  ComponentData(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
      : position(std::move(position)), rotation(std::move(rotation)),
        scale(std::move(scale)) {}
};

template <> struct ComponentData<EntityComponentType::Light> {
  glm::vec3 position;
  // TODO Add more attributes, and move position to transform
  ComponentData() = default;
  ComponentData(glm::vec3 position) : position(std::move(position)) {}
};

template <> struct ComponentData<EntityComponentType::Camera> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

template <> struct ComponentData<EntityComponentType::Renderer> {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};

// Continue with other component types

using ComponentVariant =
    std::variant<ComponentData<EntityComponentType::Transform>,
                 ComponentData<EntityComponentType::Camera>,
                 ComponentData<EntityComponentType::Light>,
                 ComponentData<EntityComponentType::Renderer>
                 // ... Add more component types.
                 >;

struct GameOjbectComponent {
  EntityComponentType type;
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