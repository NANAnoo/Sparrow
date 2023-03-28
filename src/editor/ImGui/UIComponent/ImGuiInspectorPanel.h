/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
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
	void Draw() override
	{
        if (m_Entity != nullptr) 
        {
            ImGui::Text("Name:"); ImGui::SameLine();
            std::string name = m_Entity->component<SPW::NameComponent>()->getName();
            char nameBuffer[256];
            strcpy(nameBuffer, m_Entity->component<SPW::NameComponent>()->getName().c_str());
            ImGui::InputText("*", nameBuffer, 256);
            if (ImGui::Button("Update Name"))
            {
                name.assign(nameBuffer);
                m_Entity->component<SPW::NameComponent>()->updateName(name);
                std::cout << "Update name to " << m_Entity->component<SPW::NameComponent>()->getName() << std::endl;
            }
            if (m_Entity->has<SPW::TransformComponent>()) 
                DrawTransformComponent(m_Entity->component<SPW::TransformComponent>());
            if (m_Entity->has<SPW::CameraComponent>())
                DrawCameraComponent(m_Entity->component<SPW::CameraComponent>());
            if (m_Entity->has<SPW::PointLightComponent>())
                DrawPointLightComponent(m_Entity->component<SPW::PointLightComponent>());
            if (m_Entity->has<SPW::DirectionalLightComponent>())
                DrawDirectionalLightComponent(m_Entity->component<SPW::DirectionalLightComponent>());

            ImGui::Button("Add Component", ImVec2(200, 20));
        }
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
        ImGui::PushID("Camera");
        if (ImGui::TreeNode("Camera")) /* TODO: add icon*/
        {
            if (ImGui::BeginChild("Camera", ImVec2(0, 120), true))
            {
                if(component->getType() == CameraType::PerspectiveType)
                {
                    // draw component properties
                    ImGui::InputFloat("FOV", &component->fov);
                    ImGui::InputFloat("Aspect", &component->aspect);
                    ImGui::InputFloat("Near", &component->near);
                    ImGui::InputFloat("Far", &component->far);
                }
                else if(component->getType() == CameraType::OrthoType)
                {
                    // draw component properties
                    ImGui::InputFloat("Left", &component->left);
                    ImGui::InputFloat("Right", &component->right);
                    ImGui::InputFloat("Bottom", &component->bottom);
                    ImGui::InputFloat("Top", &component->top);
                }
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
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

	void DrawDirectionalLightComponent(SPW::DirectionalLightComponent* component) const
    {
        ImGui::PushID("Dictional Light");
        if (ImGui::TreeNode("Dictional Light")) /* TODO: add icon*/
        {
            if (ImGui::BeginChild("Dictional Light", ImVec2(0, 90), true))
            {
                // draw component properties
                ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
                ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
                ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
    
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    //Key Event

    const SPW::Entity* m_Entity = nullptr;
};

}
