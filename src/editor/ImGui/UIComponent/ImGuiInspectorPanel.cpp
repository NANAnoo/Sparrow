#include "ImGuiInspectorPanel.h"

namespace SPW
{
	void ImGuiInspectorPanel::Draw()
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
				m_Entity->component<NameComponent>()->updateName(name);
				std::cout << "Update name to " << m_Entity->component<SPW::NameComponent>()->getName() << std::endl;
			}
			if (m_Entity->has<TransformComponent>())
				DrawTransformComponent(m_Entity->component<TransformComponent>());
			if (m_Entity->has<MeshComponent>())
				DrawMeshComponent(m_Entity->component<MeshComponent>());
			if (m_Entity->has<CameraComponent>())
				DrawCameraComponent(m_Entity->component<SPW::CameraComponent>());
			if (m_Entity->has<SPW::PointLightComponent>())
				DrawPointLightComponent(m_Entity->component<SPW::PointLightComponent>());
			if (m_Entity->has<SPW::DirectionalLightComponent>())
				DrawDirectionalLightComponent(m_Entity->component<SPW::DirectionalLightComponent>());
			//TODO：音效
			if(m_Entity->has<SPW::AudioComponent>())
				DrawAudioComponent(m_Entity->component<SPW::AudioComponent>());
			if(m_Entity->has<SPW::AudioListener>())
				DrawAudioListener(m_Entity->component<SPW::AudioListener>());
			//TODO: Key
			if(m_Entity->has<SPW::KeyComponent>())
				DrawKeyComponent(m_Entity->component<SPW::KeyComponent>());

			ImGui::Button("Add Component", ImVec2(200, 20));
		}
	}

	void ImGuiInspectorPanel::DrawTransformComponent(TransformComponent* component) const
	{
		ImGui::PushID("Transform");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
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
	void ImGuiInspectorPanel::DrawMeshComponent(MeshComponent* component) const
	{
		ImGui::PushID("Mesh Filter");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
		if (ImGui::TreeNode("Mesh Filter"))
		{
			// draw component properties
			for(uint32_t i =0 ; i < component->meshes.size(); ++i)
			{
				std::string label = std::string("SubMesh") + std::to_string(i + 1);
				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::BeginChild("SubMesh", ImVec2(0, 60), true))
					{
						ImGui::Text("Vertice Size: %d", component->meshes[i].vertices.size());
						ImGui::Text("Indices Size: %d", component->meshes[i].indices.size());
						ImGui::EndChild();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::PushID("Mesh Renderer");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
		if (ImGui::TreeNode("Mesh Renderer"))
		{
			// draw component properties
			for (uint32_t i = 0; i < component->materials.size(); ++i)
			{
				std::string label = std::string("Material") + std::to_string(i + 1);
				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::BeginChild("Material", ImVec2(0, 240), true))
					{
						ImGui::Text("ID %s", component->materials[i].ID.c_str());
						for(const auto&[k, v] : component->materials[i].m_TextureIDMap)
						{
							const int64_t icon_id = m_IconManager->GenerateTextureID(FileSystem::JoinPaths(FileRoots::k_Root, component->textures.find(v)->second));
							if(k == TextureMapType::Albedo)
							{
								ImGui::Text("Albedo"); ImGui::SameLine();
								ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24,24));
							}
							else if(k == TextureMapType::Normal)
							{
								ImGui::Text("Normal"); ImGui::SameLine();
								ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
							}
							else if(k == TextureMapType::Metalness)
							{
								ImGui::Text("Metalness"); ImGui::SameLine();
								ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
							}
							else if(k == TextureMapType::Roughness)
							{
								ImGui::Text("Roughness"); ImGui::SameLine();
								ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
							}
							else if(k == TextureMapType::AmbientOcclusion)
							{
								ImGui::Text("AO"); ImGui::SameLine();
								ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
							}
						}

						auto& curr_props = component->materials[i].m_Properties;
						ImGui::InputFloat3("AlbedoConstant", glm::value_ptr(curr_props.albedoConstant));
						ImGui::InputFloat3("Diffuse", glm::value_ptr(curr_props.diffuse));
						ImGui::InputFloat3("Specular", glm::value_ptr(curr_props.specular));
						ImGui::InputFloat("SpecularPower", &curr_props.specularPower);
						ImGui::InputFloat3("Ambient", glm::value_ptr(curr_props.ambient));
						ImGui::InputFloat3("Emissive", glm::value_ptr(curr_props.emissive));
						ImGui::InputFloat("EmissiveIntensity", &curr_props.emissiveIntensity);

						ImGui::InputFloat("RoughnessConstant", &curr_props.roughnessConstant);
						ImGui::InputFloat("MetallicConstant", &curr_props.metallicConstant);
						ImGui::InputFloat("AOConstant", &curr_props.AOConstant);

						ImGui::InputFloat("RefractionIntensity", &curr_props.refractionIntensity);
						ImGui::InputFloat("TransparentIntensity", &curr_props.transparentIntensity);


						ImGui::EndChild();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	void ImGuiInspectorPanel::DrawCameraComponent(CameraComponent* component) const
	{
		ImGui::PushID("Camera");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
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

	void ImGuiInspectorPanel::DrawPointLightComponent(PointLightComponent* component) const
	{
		ImGui::PushID("Point Light");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
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
	void ImGuiInspectorPanel::DrawDirectionalLightComponent(DirectionalLightComponent* component) const
	{
		ImGui::PushID("Dictional Light");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
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
	void ImGuiInspectorPanel::DrawAudioComponent(AudioComponent* component) const
	{
		ImGui::PushID("Audio Source");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
		if (ImGui::TreeNode("Audio Source")) /* TODO: add icon*/
		{
			int i = 1,j = 1;
			for(auto &a : component->allSounds){
				std::string clipNum = "Audio Clip " + std::to_string(i++);
				std::string AudioSourceNum = "Audio Source " + std::to_string(j++);
				if (ImGui::TreeNode(AudioSourceNum.c_str())){
					ImGui::InputText(clipNum.c_str(),const_cast<char*>(a.first.c_str()),256);
					ImGui::Checkbox("is 3D",&a.second->is3D);
					ImGui::Checkbox("is Loop",&a.second->isLoop);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::TreePop();
				}

			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	void ImGuiInspectorPanel::DrawAudioListener(AudioListener* component) const
	{
		ImGui::PushID("Audio");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
		if (ImGui::TreeNode("Audio")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("AudioSource", ImVec2(0, 90), true)) {
				// draw component properties
				ImGui::InputInt("Listener ID",&component->Listener_id);

				ImGui::EndChild();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	void ImGuiInspectorPanel::DrawKeyComponent(KeyComponent* component) const
	{
		ImGui::PushID("Event System");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize); ImGui::SameLine();
		if (ImGui::TreeNode("Event System")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("Event System", ImVec2(0, 90), true)) {
				// draw component properties
				ImGui::Checkbox("Key Enable",&component->keyEnable);

				ImGui::EndChild();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}