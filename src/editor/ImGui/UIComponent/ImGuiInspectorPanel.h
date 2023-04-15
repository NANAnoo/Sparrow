/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/Lights/DirectionalLightComponent.hpp"
#include "EcsFramework/Component/Lights/PointLightComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/Audio/AudioComponent.h"
#include "EcsFramework/Component/Audio/AudioListener.h"
#include "EcsFramework/Component/KeyComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "stb_image.h"

#include "ImGui/ImGuiIconManager.hpp"
#include "ImGui/ImGuiMessageBox/ImGuiMessageBox.h"
#include "IO/ResourceManager.h"
#include "IO/FileSystem.h"

namespace SPW
{
	class ImGuiInspectorPanel : public ImGuiPanel
	{
	public:
		ImGuiInspectorPanel(std::string title, ImGuiIconManager* iconManager, bool *open = nullptr)
			: ImGuiPanel(std::move(title), open)
			, m_IconManager(iconManager)
		{
		}

	  	void SetSelectedGameObject(const Entity& e) {m_Entity = &e;}

	protected:
		void Draw() override;

	private:
		void DrawTransformComponent(TransformComponent* component) const;
		void DrawMeshComponent(MeshComponent* component);
		void DrawCameraComponent(CameraComponent* component) const;
		void DrawPointLightComponent(PointLightComponent* component) const;
		void DrawDirectionalLightComponent(DirectionalLightComponent* component) const;
		void DrawAudioComponent(AudioComponent* component) const;
		void DrawAudioListener(AudioListener* component) const;
		void DrawKeyComponent(KeyComponent* component) const;

	private:
		const Entity* m_Entity = nullptr;
		ImGuiIconManager* m_IconManager;
		ImVec2 k_DefalutImageSize = ImVec2(20, 20);
		std::unique_ptr<ImGuiMessageBox> msgBox_Inspector;

		bool show_panel = false;
	};

}
