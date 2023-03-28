/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
#include "assimp/code/Common/Win32DebugLogStream.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"

namespace SPW {

class ImGuiInspectorPanel : public ImGuiPanel {
public:
  ImGuiInspectorPanel(std::string title, bool *open = nullptr)
      : ImGuiPanel(std::move(title), open)
	{}

  void SetSelectedGameObject(const Entity& e) {m_Entity = &e;}

protected:
	void Draw() override {
        if (m_Entity != nullptr) 
        {
            ImGui::Text("Name: %s", m_Entity->component<SPW::NameComponent>()->getName().c_str());
            if (m_Entity->has<SPW::TransformComponent>()) 
                DrawTransformComponent(m_Entity->component<SPW::TransformComponent>());
            if (m_Entity->has<SPW::CameraComponent>())
                DrawCameraComponent(m_Entity->component<SPW::CameraComponent>());
            if (m_Entity->has<SPW::PointLightComponent>())
                DrawPointLightComponent(m_Entity->component<SPW::PointLightComponent>());
            if (m_Entity->has<SPW::DirectionalLightComponent>()) {
                ImGui::Text("Name: %s", m_Entity->component<SPW::NameComponent>()->getName().c_str());
            }
            else {
                ImGui::Text("Unknown");
            }
        }
	  
	  // ;
    // DrawComponents(std::forward<std::shared_ptr<Entity>>(m_Entity));
  }

private:

    void DrawTransformComponent(SPW::TransformComponent* component) const
    {
        ImGui::PushID("Transform"); 
        if (ImGui::TreeNode("Transform")) /* TODO: add icon*/
        {
            if (ImGui::BeginChild("Transform", ImVec2(0, 90), true)) {
                // draw component properties
                ImGui::InputFloat3("Position", glm::value_ptr(component->position));
                ImGui::InputFloat3("Rotation", glm::value_ptr(component->rotation));
                ImGui::InputFloat3("Scale", glm::value_ptr(component->scale));

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

	void DrawCameraComponent(SPW::CameraComponent* component) const
    {
        // ImGui::PushID("Camera"); 
        // if (ImGui::TreeNode("Camera")) /* TODO: add icon*/
        // {
        //     if (ImGui::BeginChild("Camera", ImVec2(0, 120), true)) 
        //     {
        //         if(component->getType() == CameraType::PerspectiveType)
        //         {
        //             // draw component properties
        //             ImGui::InputFloat("FOV", &component->fov);
        //             ImGui::InputFloat("Aspect", &component->aspect);
        //             // ImGui::InputFloat("Near", &component->near);
        //             // ImGui::InputFloat("far", &component->far);
        //         }
        //         else if(component->getType() == CameraType::OrthoType)
        //         {
        //             // draw component properties
        //             ImGui::InputFloat("Left", &component->left);
        //             ImGui::InputFloat("Right", &component->right);
        //             ImGui::InputFloat("Bottom", &component->bottom);
        //             ImGui::InputFloat("Top", &component->top);
        //         }
        //         ImGui::EndChild();
        //     }
        //     ImGui::TreePop();
        // }
        // ImGui::PopID();
    }

    void DrawPointLightComponent(SPW::PointLightComponent* component) const
    {
        ImGui::PushID("Point Light"); 
        if (ImGui::TreeNode("Point Light")) /* TODO: add icon*/
        {
            if (ImGui::BeginChild("Point Light", ImVec2(0, 180), true)) 
            {
                // draw component properties
                ImGui::InputFloat("Constant", &component->constant);
                ImGui::InputFloat("Linear", &component->linear);
                ImGui::InputFloat("Quadratic", &component->quadratic);
                ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
                ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
                ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
 
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

	// template <>
	// void DrawComponentByType<EntityComponentType::Transform>(ComponentI& component)
	// {

	// }

  // template <>
  // void DrawComponentByType<EntityComponentType::Camera>(SPW::CameraComponent& component) {
  //   ImGui::PushID("Camera");
  //   // Draw the Camera component
  //   ImGui::PopID();
  // }
  //
  // template <>
  // void DrawComponentByType<EntityComponentType::Light>(
  //     GameOjbectComponent &component) {
  //   // Get the Transform component data
  //   auto &lgihtData =
  //       std::get<ComponentData<EntityComponentType::Light>>(component.data);
  //
  //   ImGui::PushID("Light");
  //   if (ImGui::TreeNode("Light")) /* Enable Tree View */
  //   {
  //     if (ImGui::BeginChild("Light", ImVec2(0, 35), true)) {
  //       // Draw the Transform component properties
  //       ImGui::InputFloat3("Position", glm::value_ptr(lgihtData.position));
  //
  //       // ImGui::Text("Component: %s", component.name.c_str());
  //       if (component.callback && ImGui::IsItemClicked()) {
  //         component.callback();
  //       }
  //       ImGui::EndChild();
  //     }
  //     ImGui::TreePop();
  //   }
  //
  //   ImGui::PopID();
  // }
  //
  // template <>
  // void DrawComponentByType<EntityComponentType::Renderer>(
  //     GameOjbectComponent &component) {
  //   ImGui::PushID("Renderer");
  //   // Draw the Camera component
  //   ImGui::PopID();
  // }

  const SPW::Entity* m_Entity = nullptr;
};

}
