#include "ImGuiInspectorPanel.h"
#include "EcsFramework/Component/ComponentTypes.h"
namespace SPW
{
	void ImGuiInspectorPanel::Draw()
	{
		if (m_Entity != nullptr)
		{
			// ------------- UPDATE NAME ------------- 
			std::string name = m_Entity->component<SPW::NameComponent>()->getName();
			ImGui::Text("Name: %s", name.c_str());
			// bool show_naming = false;
			if (ImGui::Button("Upate Name"))
			{
				show_naming = true;
			}
			if (show_naming)
			{
				// strcpy(m_PendingName, name.c_str());
				ImGui::Begin("Update Name", &show_naming);
				ImGui::Text("Enter your new name: ");
				ImGui::SameLine();
				ImGui::InputText("*", m_PendingName, sizeof(m_PendingName));
				if (ImGui::Button("Update"))
				{
					m_Entity->component<SPW::NameComponent>()->updateName(convertToString(m_PendingName));
					show_naming = false;
					strcpy(m_PendingName, " ");
				}
				ImGui::End();
			}
			// ------------- UPDATE NAME ------------- 


			// ------------- LOOP COMPONENTS -------------
			componentStatus.at(ComponentType::IDComponent) = true;
			componentStatus.at(ComponentType::NameComponent) = true;
			if (m_Entity->has<TransformComponent>())
				componentStatus.at(ComponentType::TransformComponent) = true;
			if (m_Entity->has<MeshComponent>())
				componentStatus.at(ComponentType::MeshComponent) = true;
			if (m_Entity->has<CameraComponent>())
				componentStatus.at(ComponentType::CameraComponent) = true;
			if (m_Entity->has<PointLightComponent>())
				componentStatus.at(ComponentType::PointLightComponent) = true;
			if (m_Entity->has<DirectionalLightComponent>())
				componentStatus.at(ComponentType::DirectionalLightComponent) = true;
			if (m_Entity->has<AudioComponent>())
				componentStatus.at(ComponentType::AudioComponent) = true;
			if (m_Entity->has<AudioListener>())
				componentStatus.at(ComponentType::AudioListener) = true;
			if (m_Entity->has<KeyComponent>())
				componentStatus.at(ComponentType::KeyComponent) = true;
			if (m_Entity->has<MouseComponent>())
				componentStatus.at(ComponentType::MouseComponent) = true;
			// ------------- LOOP COMPONENTS -------------

			// ------------- RENDER COMPONENTS -------------
			if (m_Entity->has<TransformComponent>())
				DrawTransformComponent(m_Entity->component<TransformComponent>());
			if (m_Entity->has<MeshComponent>())
				DrawMeshComponent(m_Entity->component<MeshComponent>());
			if (m_Entity->has<CameraComponent>())
				DrawCameraComponent(m_Entity->component<CameraComponent>());
//			if (m_Entity->has<PointLightComponent>())
//				DrawPointLightComponent(m_Entity->component<PointLightComponent>());
//			if (m_Entity->has<DirectionalLightComponent>())
//				DrawDirectionalLightComponent(m_Entity->component<DirectionalLightComponent>());
			if (m_Entity->has<PointLightComponent>() || m_Entity->has<DirectionalLightComponent>())
				DrawLightComponent();
			if (m_Entity->has<AudioComponent>())
				DrawAudioComponent(m_Entity->component<AudioComponent>());
			if (m_Entity->has<AudioListener>())
				DrawAudioListener(m_Entity->component<AudioListener>());
			if (m_Entity->has<KeyComponent>())
				DrawKeyComponent(m_Entity->component<KeyComponent>());
			// ------------- RENDER COMPONENTS -------------

			// ------------- ADD ANY COMPONENT ------------- 
			if (ImGui::Button("Add Component", ImVec2(200, 20)))
			{
				show_addcomponent = true;
			}

			if (show_addcomponent)
			{
				ImGui::Begin("Add Component");
				for (const auto& [componentType, status]: componentStatus)
				{
					if (!status)
					{
						std::string label = ComponentTypeToString(componentType);
						if(label == "Unknown") continue;

						if( ImGui::Button( label.c_str() ) )
						{
							if(componentType == ComponentType::TransformComponent)
							{
								std::cout << "Add TransformComponent\n";
								m_Entity->emplace<TransformComponent>();
							}
							else if(componentType == ComponentType::MeshComponent /*&& componentStatus[ComponentType::CameraComponent]*/)
							{
								std::cout << "Add MeshComponent\n";
								m_Entity->emplace<MeshComponent>();
							}
							else if(componentType == ComponentType::CameraComponent)
							{
								std::cout << "Add CameraComponent\n";
								m_Entity->emplace<CameraComponent>();
							}
							else if(componentType == ComponentType::PointLightComponent)
							{
								std::cout << "Add Point Light Component\n";
								m_Entity->emplace<PointLightComponent>();
							}
							else if(componentType == ComponentType::DirectionalLightComponent)
							{
								std::cout << "Add DirectionalLight Component\n";
								m_Entity->emplace<DirectionalLightComponent>();
							}
							else if(componentType == ComponentType::KeyComponent)
							{
								std::cout << "Add KeyComponent Component\n";
								// TODO
								m_Entity->emplace<SPW::KeyComponent>();
							}
							else if(componentType == ComponentType::MouseComponent)
							{
								std::cout << "Add MouseComponent Component\n";
								m_Entity->emplace<SPW::MouseComponent>();
							}
							else if(componentType == ComponentType::AudioComponent)
							{
								std::cout << "Add AudioComponent Component\n";
								m_Entity->emplace<SPW::AudioComponent>();
							}
							else if(componentType == ComponentType::AudioListener)
							{
								m_Entity->emplace<AudioListener>();
								std::cout << "Add AudioListener Component\n";
							}

							show_addcomponent = false;
						}
					}
				}
				if( ImGui::Button( "Cancel" ) ) { show_addcomponent = false; }

				ImGui::End();
			}
			// ------------- ADD ANY COMPONENT ------------- 
		}
	}

	void ImGuiInspectorPanel::DrawTransformComponent(TransformComponent* component) const
	{
		ImGui::PushID("Transform");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();
		if (ImGui::TreeNode("Transform")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("Transform", ImVec2(0, 90), true))
			{
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

	void ImGuiInspectorPanel::DrawMeshComponent(MeshComponent* component)
	{
		// ------------------------- SWITCH&UPDATE LOGIC -------------------------
		std::vector<const char*> captions;

		if (ImGui::Button("Select Mesh!"))
		{
			show_panel = true;
		}

		// if (msgBox_Inspector->trigger_flag)
		if (show_panel)
		{
			ImGui::Begin("Select Your Mesh", &show_panel, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Separator();
			if (!component->assetName.empty())
			{
				for (const auto& [k, v]: ResourceManager::getInstance()->m_AssetDataMap)
				{
					if (ImGui::Button(k.c_str()))
					{
						component->assetID = v.assetID;
						component->assetName = v.assetName;
						component->assetPath = v.path;

						show_panel = false;
					}
				}
			}
			ImGui::Text("dude, It is not a valid mesh Component");

			ImGui::End();
		}

		// ------------------------- DISPLAY LOGIC -------------------------
		if (!component->assetName.empty())
		{
			auto& active_asset_data = ResourceManager::getInstance()->m_AssetDataMap[component->assetName];

			ImGui::PushID("Asset Meta");
			ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
			ImGui::SameLine();
			std::string label = "Asset Meta: " + component->assetName;
			if (ImGui::TreeNode(label.c_str()))
			{
				if (ImGui::BeginChild("Asset Meta", ImVec2(0, 60), true))
				{
					ImGui::Text("assetID:   %s", component->assetID.c_str());
					ImGui::Text("assetName: %s", component->assetName.c_str());
					ImGui::Text("assetPath: %s", component->assetPath.c_str());
					ImGui::EndChild();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::PushID("Mesh Filter");
			ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
			ImGui::SameLine();
			if (ImGui::TreeNode("Mesh Filter"))
			{
				// Add a child window with scrolling capabilities
				ImGui::BeginChild("MeshFilterNoScrolling", ImVec2(0, 100), false,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

				// draw component properties
				for (uint32_t i = 0; i < active_asset_data.meshes.size(); ++i)
				{
					std::string label = std::string("SubMesh") + std::to_string(i + 1);
					if (ImGui::CollapsingHeader(label.c_str()/*, ImGuiTreeNodeFlags_DefaultOpen*/))
					{
						ImGui::Text("Vertice Size: %d", active_asset_data.meshes[i].vertices.size());
						ImGui::Text("Indices Size: %d", active_asset_data.meshes[i].indices.size());
					}
				}

				// End the scrolling child window
				ImGui::EndChild();

				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::PushID("Mesh Renderer");
			ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
			ImGui::SameLine();
			if (ImGui::TreeNode("Mesh Renderer"))
			{
				if (!component->assetName.empty())
				{
					// draw component properties
					for (uint32_t i = 0; i < active_asset_data.materials.size(); ++i)
					{
						std::string label = std::string("Material") + std::to_string(i + 1);
						if (ImGui::CollapsingHeader(label.c_str()/*, ImGuiTreeNodeFlags_DefaultOpen*/))
						{
							// if (ImGui::BeginChild("Material", ImVec2(0, 240), true))
							{
								ImGui::Text("ID %s", active_asset_data.materials[i].ID.c_str());
								for (const auto& [k, v]: active_asset_data.materials[i].m_TextureIDMap)
								{
									const int64_t icon_id = m_IconManager
										->GenerateIconID(FileSystem::JoinPaths(FileRoots::k_Root,
											active_asset_data.textures.find(v)->
												second));
									if (k == TextureMapType::Albedo)
									{
										ImGui::Text("Albedo");
										ImGui::SameLine();
										ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
									}
									else if (k == TextureMapType::Normal)
									{
										ImGui::Text("Normal");
										ImGui::SameLine();
										ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
									}
									else if (k == TextureMapType::Metalness)
									{
										ImGui::Text("Metalness");
										ImGui::SameLine();
										ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
									}
									else if (k == TextureMapType::Roughness)
									{
										ImGui::Text("Roughness");
										ImGui::SameLine();
										ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
									}
									else if (k == TextureMapType::AmbientOcclusion)
									{
										ImGui::Text("AO");
										ImGui::SameLine();
										ImGui::Image(reinterpret_cast<void*>(icon_id), ImVec2(24, 24));
									}
								}

								auto& curr_props = active_asset_data.materials[i].m_Properties;
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

								// ImGui::EndChild();
							}
							// ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::PopID();
		}
	}

	void ImGuiInspectorPanel::DrawCameraComponent(CameraComponent* component) const
	{
		ImGui::PushID("Camera");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();
		if (ImGui::TreeNode("Camera")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("Camera", ImVec2(0, 120), true))
			{
				if (component->getType() == CameraType::PerspectiveType)
				{
					// draw component properties
					ImGui::InputFloat("FOV", &component->fov);
					ImGui::InputFloat("Aspect", &component->aspect);
					ImGui::InputFloat("Near", &component->near);
					ImGui::InputFloat("Far", &component->far);
				}
				else if (component->getType() == CameraType::OrthoType)
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

//	void ImGuiInspectorPanel::DrawPointLightComponent(PointLightComponent* component) const
//	{
//		ImGui::PushID("Point Light");
//		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
//		ImGui::SameLine();
//		if (ImGui::TreeNode("Point Light")) /* TODO: add icon*/
//		{
//			if (ImGui::BeginChild("Point Light", ImVec2(0, 180), true))
//			{
//				// draw component properties
//				ImGui::InputFloat("Constant", &component->constant);
//				ImGui::InputFloat("Linear", &component->linear);
//				ImGui::InputFloat("Quadratic", &component->quadratic);
//				ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
//				ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
//				ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
//
//				ImGui::EndChild();
//			}
//			ImGui::TreePop();
//		}
//		ImGui::PopID();
//	}

//	void ImGuiInspectorPanel::DrawDirectionalLightComponent(DirectionalLightComponent* component) const
//	{
//		ImGui::PushID("Dictional Light");
//		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
//		ImGui::SameLine();
//		if (ImGui::TreeNode("Dictional Light")) /* TODO: add icon*/
//		{
//			if (ImGui::BeginChild("Dictional Light", ImVec2(0, 90), true))
//			{
//				// draw component properties
//				ImGui::InputFloat3("Ambient", glm::value_ptr(component->ambient));
//				ImGui::InputFloat3("Diffuse", glm::value_ptr(component->diffuse));
//				ImGui::InputFloat3("Specular", glm::value_ptr(component->specular));
//
//				ImGui::EndChild();
//			}
//			ImGui::TreePop();
//		}
//		ImGui::PopID();
//	}

	void ImGuiInspectorPanel::DrawLightComponent() const
	{
		ImGui::PushID("Light");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();

		//  PointLightComponent = 0, DirectionalLightComponent = 1;
		const char* items[] = {"Point Light", "Directional Light"};
		if (ImGui::TreeNode("Light Source"))
		{
			int orignal_lightType = m_Entity->has<PointLightComponent>() ? 0 : 1;
			ImGui::SameLine(); if(ImGui::Button("x"))
			{
				if(orignal_lightType == 0)
				{
					m_Entity->remove<PointLightComponent>();
				}
				else
				{
					m_Entity->remove<DirectionalLightComponent>();
				}

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			int current_lightType = orignal_lightType;
			ImGui::Text("Light Type: "); ImGui::SameLine();

			if (ImGui::BeginCombo(" ", items[current_lightType]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(items); i++)
				{
					bool isSelected = (current_lightType == i);
					if (ImGui::Selectable(items[i], isSelected))
					{
						current_lightType = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if( current_lightType != orignal_lightType )
			{
				if (current_lightType == 0) // PointLight
				{
					m_Entity->remove<DirectionalLightComponent>();
					m_Entity->emplace<PointLightComponent>();
				}
				if (current_lightType == 1) // DirectionalLight
				{
					m_Entity->remove<PointLightComponent>();
					m_Entity->emplace<DirectionalLightComponent>();
				}
			}

			if( m_Entity->has<PointLightComponent>() )
			{
				auto&& pointlight_component = m_Entity->component<PointLightComponent>();
				if (ImGui::BeginChild("Point Light", ImVec2(0, 180), true))
				{
					// draw component properties
					ImGui::InputFloat("Constant", &pointlight_component->constant);
					ImGui::InputFloat("Linear", &pointlight_component->linear);
					ImGui::InputFloat("Quadratic", &pointlight_component->quadratic);
					ImGui::InputFloat3("Ambient", glm::value_ptr(pointlight_component->ambient));
					ImGui::InputFloat3("Diffuse", glm::value_ptr(pointlight_component->diffuse));
					ImGui::InputFloat3("Specular", glm::value_ptr(pointlight_component->specular));

					ImGui::EndChild();
				}
				ImGui::TreePop();
			}

			if( m_Entity->has<DirectionalLightComponent>() )
			{
				auto&& directionallight_component = m_Entity->component<DirectionalLightComponent>();
				if (ImGui::BeginChild("Directional Light", ImVec2(0, 90), true))
				{
					// draw component properties
					ImGui::InputFloat3("Ambient", glm::value_ptr(directionallight_component->ambient));
					ImGui::InputFloat3("Diffuse", glm::value_ptr(directionallight_component->diffuse));
					ImGui::InputFloat3("Specular", glm::value_ptr(directionallight_component->specular));

					ImGui::EndChild();
				}
				ImGui::TreePop();
			}
		}
		ImGui::PopID();
	}

	void ImGuiInspectorPanel::DrawAudioComponent(AudioComponent* component) const
	{
		ImGui::PushID("Audio Source");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();
		if (ImGui::TreeNode("Audio Source")) /* TODO: add icon*/
		{
			int i = 1, j = 1;
			for (auto& a: component->allSounds)
			{
				std::string clipNum = "Audio Clip " + std::to_string(i++);
				std::string AudioSourceNum = "Audio Source " + std::to_string(j++);
				if (ImGui::TreeNode(AudioSourceNum.c_str()))
				{
					ImGui::InputText(clipNum.c_str(), const_cast<char*>(a.first.c_str()), 256);
					ImGui::Checkbox("is 3D", &a.second->is3D);
					ImGui::Checkbox("is Loop", &a.second->isLoop);
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
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();
		if (ImGui::TreeNode("Audio")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("AudioSource", ImVec2(0, 90), true))
			{
				// draw component properties
				ImGui::InputInt("Listener ID", &component->Listener_id);

				ImGui::EndChild();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	void ImGuiInspectorPanel::DrawKeyComponent(KeyComponent* component) const
	{
		ImGui::PushID("Event System");
		ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		ImGui::SameLine();
		if (ImGui::TreeNode("Event System")) /* TODO: add icon*/
		{
			if (ImGui::BeginChild("Event System", ImVec2(0, 90), true))
			{
				// draw component properties
				ImGui::Checkbox("Key Enable", &component->keyEnable);

				ImGui::EndChild();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}
