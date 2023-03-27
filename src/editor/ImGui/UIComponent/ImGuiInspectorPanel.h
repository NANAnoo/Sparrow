/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"

namespace SPW {

class ImGuiInspectorPanel : public ImGuiPanel {
public:
  ImGuiInspectorPanel(std::string title, bool *open = nullptr)
      : ImGuiPanel(std::move(title), open) {}

  void
  SetSelectedGameObject(const std::shared_ptr<GameObject> &gameObject_ptr) {
    m_GameObject = gameObject_ptr;
  }

protected:
  void Draw() override {
    if (m_GameObject != nullptr) {
      ImGui::Text("Name: %s", m_GameObject->name.c_str());

      DrawComponents(m_GameObject->components);
    }
  }

private:
  void DrawComponents(std::vector<GameOjbectComponent> &components) {
    for (auto &component : components) {
      switch (component.type) {
      case GameOjbectComponentType::Transform:
        DrawComponentByType<GameOjbectComponentType::Transform>(component);
        break;
      case GameOjbectComponentType::Camera:
        DrawComponentByType<GameOjbectComponentType::Camera>(component);
        break;
      case GameOjbectComponentType::Light:
        DrawComponentByType<GameOjbectComponentType::Light>(component);
        break;
      case GameOjbectComponentType::Renderer:
        DrawComponentByType<GameOjbectComponentType::Renderer>(component);
        break;
        // ... Handle other component types
      default:
        "Unknown component type";
        break;
      }
    }
    ImGui::Button("Add Component", ImVec2(200, 20));
  }

  template <GameOjbectComponentType T>
  void DrawComponentByType(GameOjbectComponent &component);

  template <>
  void DrawComponentByType<GameOjbectComponentType::Transform>(
      GameOjbectComponent &component) {
    // Get the Transform component data
    auto &transformData =
        std::get<ComponentData<GameOjbectComponentType::Transform>>(
            component.data);

    ImGui::PushID("Transform");
    if (ImGui::TreeNode("Transform")) /* Enable Tree View */
    {
      if (ImGui::BeginChild("Transform", ImVec2(0, 100), true)) {
        // Draw the Transform component properties
        ImGui::InputFloat3("Position", glm::value_ptr(transformData.position));
        ImGui::InputFloat3("Rotation", glm::value_ptr(transformData.rotation));
        ImGui::InputFloat3("Scale", glm::value_ptr(transformData.scale));

        // ImGui::Text("Component: %s", component.name.c_str());
        if (component.callback && ImGui::IsItemClicked()) {
          component.callback();
        }
        ImGui::EndChild();
      }
      ImGui::TreePop();
    }
    ImGui::PopID();
  }

  template <>
  void DrawComponentByType<GameOjbectComponentType::Camera>(
      GameOjbectComponent &component) {
    ImGui::PushID("Camera");
    // Draw the Camera component
    ImGui::PopID();
  }

  template <>
  void DrawComponentByType<GameOjbectComponentType::Light>(
      GameOjbectComponent &component) {
    // Get the Transform component data
    auto &lgihtData =
        std::get<ComponentData<GameOjbectComponentType::Light>>(component.data);

    ImGui::PushID("Light");
    if (ImGui::TreeNode("Light")) /* Enable Tree View */
    {
      if (ImGui::BeginChild("Light", ImVec2(0, 35), true)) {
        // Draw the Transform component properties
        ImGui::InputFloat3("Position", glm::value_ptr(lgihtData.position));

        // ImGui::Text("Component: %s", component.name.c_str());
        if (component.callback && ImGui::IsItemClicked()) {
          component.callback();
        }
        ImGui::EndChild();
      }
      ImGui::TreePop();
    }

    ImGui::PopID();
  }

  template <>
  void DrawComponentByType<GameOjbectComponentType::Renderer>(
      GameOjbectComponent &component) {
    ImGui::PushID("Renderer");
    // Draw the Camera component
    ImGui::PopID();
  }

  std::shared_ptr<GameObject> m_GameObject;
};

}