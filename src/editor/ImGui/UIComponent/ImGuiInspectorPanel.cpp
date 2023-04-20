#include "ImGuiInspectorPanel.h"
#include "EcsFramework/Component/ComponentTypes.h"
#include "ImGui/IconsFontAwesome6.h"

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
			if (ImGui::Button("Update Name"))
			{
				show_naming = true;
			}
			if (show_naming)
			{
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
			//			if (m_Entity->has<PointLightComponent>())
			//				componentStatus.at(ComponentType::PointLightComponent) = true;
			if (m_Entity->has<DirectionalLightComponent>() || m_Entity->has<PointLightComponent>())
				componentStatus.at(ComponentType::DirectionalLightComponent) = true;
			if (m_Entity->has<AnimationComponent>())
				componentStatus.at(ComponentType::AnimationComponent) = true;
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
			if (m_Entity->has<AnimationComponent>())
				DrawAnimationComponent(m_Entity->component<AnimationComponent>());
			if (m_Entity->has<AudioComponent>())
				DrawAudioComponent(m_Entity->component<AudioComponent>());
			if (m_Entity->has<AudioListener>())
				DrawAudioListener(m_Entity->component<AudioListener>());
			if (m_Entity->has<KeyComponent>() || m_Entity->has<MouseComponent>())
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
				for (const auto& [componentType, status] : componentStatus)
				{
					if (!status)
					{
						std::string label = ComponentTypeToString(componentType);
						if (label == "Unknown") continue;

						if (ImGui::Button(label.c_str()))
						{
							if (componentType == ComponentType::TransformComponent)
							{
								std::cout << "Add TransformComponent\n";
								m_Entity->emplace<TransformComponent>();
							}
							else if (componentType == ComponentType::MeshComponent
								/*&& componentStatus[ComponentType::CameraComponent]*/)
							{
								std::cout << "Add MeshComponent\n";
								// auto transform = m_Entity->emplace<TransformComponent>();
								// transform->scale = { 0.1, 0.1, 0.1 };
								// transform->rotation = { 0, 90, 0 };
								// transform->position = { 0, -0.3, 0 };

								const auto& rm = ResourceManager::getInstance();
								// add a model to show
								auto model = m_Entity->emplace<MeshComponent>(rm->m_CameraMap["main"]);

								model->bindRenderGraph = rm->m_RenderGraph["pbr_with_PDshadow"]->graph_id;
								model->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["p_shadowmap_node"]->pass_id] = rm->m_ShaderMap["p_shadow_desc"].uuid;
								model->modelSubPassPrograms[rm->m_ModelRepeatPassNodes["d_shadowmap_node"]->pass_id] = rm->m_ShaderMap["d_shadow_desc"].uuid;
								model->modelSubPassPrograms[rm->m_ModelToScreenNodes["pbr_shadow_lighting_node"]->pass_id] = rm->m_ShaderMap["pbr_light_shadow_desc"].uuid;

								// m_Entity->emplace<MeshComponent>();
							}
							else if (componentType == ComponentType::CameraComponent)
							{
								std::cout << "Add CameraComponent\n";
								m_Entity->emplace<CameraComponent>();
							}
							else if (componentType == ComponentType::PointLightComponent)
							{
								std::cout << "Add Point Light Component\n";
								m_Entity->emplace<PointLightComponent>();
							}
							else if (componentType == ComponentType::DirectionalLightComponent)
							{
								std::cout << "Add DirectionalLight Component\n";
								m_Entity->emplace<DirectionalLightComponent>();
							}
							else if (componentType == ComponentType::KeyComponent)
							{
								std::cout << "Add KeyComponent Component\n";
								m_Entity->emplace<SPW::KeyComponent>();
							}
							else if (componentType == ComponentType::MouseComponent)
							{
								std::cout << "Add MouseComponent Component\n";
								m_Entity->emplace<SPW::MouseComponent>();
							}
							else if (componentType == ComponentType::AudioComponent)
							{
								std::cout << "Add AudioComponent Component\n";
								m_Entity->emplace<SPW::AudioComponent>();
							}
							else if (componentType == ComponentType::AudioListener)
							{
								m_Entity->emplace<AudioListener>();
								std::cout << "Add AudioListener Component\n";
							}

							show_addcomponent = false;
						}
					}
				}
				if (ImGui::Button("Cancel")) { show_addcomponent = false; }

				ImGui::End();
			}
			// ------------- ADD ANY COMPONENT ------------- 
		}
	}

	void ImGuiInspectorPanel::DrawTransformComponent(TransformComponent* component)
	{
		ImGui::PushID("Transform");
		if (ImGui::TreeNode(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT"		Transform"))
		{
			if (ImGui::Button("delete"))
			{

				//TODO To delete the entity
//				m_Entity->remove<TransformComponent>();
//				scene_ptr->deleteEntity(m_Entity);

//				m_Entity = nullptr;
				ImGui::TreePop();
				ImGui::PopID();
				return;
			}


			if (ImGui::BeginChild("Transform", ImVec2(0, 90), true))
			{
				// draw component properties
				ImGui::DragFloat3("Position", glm::value_ptr(component->position));
				ImGui::DragFloat3("Rotation", glm::value_ptr(component->rotation));
				ImGui::DragFloat3("Scale", glm::value_ptr(component->scale));

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
			show_selectingMesh = true;
		}

		if (show_selectingMesh)
		{
			ImGui::Begin("Select Your Mesh", &show_selectingMesh, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Separator();
			// if (!component->assetName.empty())
			{
				for (const auto& [k, v] : ResourceManager::getInstance()->m_AssetDataMap)
				{
					if (!k.empty())
					{
						if (ImGui::Button(k.c_str()))
						{
							component->assetID = v.assetID;
							component->assetName = v.assetName;
							component->assetPath = v.path;
							component->ready = false;
							show_selectingMesh = false;
						}
					}
				}
			}
			// else
			// 	ImGui::Text("dude, It is not a valid mesh Component");

			ImGui::End();
		}

		if (ImGui::Button("delete"))
		{
			m_Entity->remove<MeshComponent>();
			return;
		}


		// ------------------------- DISPLAY LOGIC -------------------------
		if (!component->assetName.empty())
		{
			auto& active_asset_data = ResourceManager::getInstance()->m_AssetDataMap[component->assetName];

			ImGui::PushID("Asset Meta");
			std::string label = ICON_FA_DATABASE"		Asset Meta : " + component->assetName;
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
			if (ImGui::TreeNode(ICON_FA_SQUARE"		Mesh Filter"))
			{
				// Add a child window with scrolling capabilities
				ImGui::BeginChild("MeshFilterNoScrolling", ImVec2(0, 100), false,
				                  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

				// draw component properties
				for (uint32_t i = 0; i < active_asset_data.meshes.size(); ++i)
				{
					const std::string label = std::string("SubMesh") + std::to_string(i + 1);
					if (ImGui::CollapsingHeader(label.c_str()/*, ImGuiTreeNodeFlags_DefaultOpen*/))
					{
						ImGui::Text("Vertices Size: %zu", active_asset_data.meshes[i].vertices.size());
						ImGui::Text("Indices Size: %zu", active_asset_data.meshes[i].indices.size());
					}
				}

				// End the scrolling child window
				ImGui::EndChild();

				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::PushID("Mesh Renderer");
			if (ImGui::TreeNode(ICON_FA_PALETTE"		Mesh Renderer"))
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
								for (const auto& [k, v] : active_asset_data.materials[i].m_TextureIDMap)
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
								ImGui::DragFloat3("AlbedoConstant", glm::value_ptr(curr_props.albedoConstant));
								ImGui::DragFloat3("Diffuse", glm::value_ptr(curr_props.diffuse));
								ImGui::DragFloat3("Specular", glm::value_ptr(curr_props.specular));
								ImGui::DragFloat("SpecularPower", &curr_props.specularPower);
								ImGui::DragFloat3("Ambient", glm::value_ptr(curr_props.ambient));
								ImGui::DragFloat3("Emissive", glm::value_ptr(curr_props.emissive));
								ImGui::DragFloat("EmissiveIntensity", &curr_props.emissiveIntensity);

								ImGui::DragFloat("RoughnessConstant", &curr_props.roughnessConstant);
								ImGui::DragFloat("MetallicConstant", &curr_props.metallicConstant);
								ImGui::DragFloat("AOConstant", &curr_props.AOConstant);

								ImGui::DragFloat("RefractionIntensity", &curr_props.refractionIntensity);
								ImGui::DragFloat("TransparentIntensity", &curr_props.transparentIntensity);

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

	void ImGuiInspectorPanel::DrawAnimationComponent(AnimationComponent* component) const
	{
		ImGui::PushID("Animation");

		if (ImGui::TreeNode(ICON_FA_PERSON_RUNNING"		Animation")) /* TODO: add icon*/
		{
			if (ImGui::Button("delete"))
			{
				m_Entity->remove<AnimationComponent>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			if (ImGui::BeginChild("Animation", ImVec2(0, 120), true))
			{
				for(const auto& anim : component->skeleton->animClips)
				{
					const char* anim_name = anim.name.c_str();
					ImGui::Text("name : %s", anim_name);
				}

				ImGui::Checkbox("onGoingAnim update", &component->onGoingAnim->bUpdate);
				
				ImGui::Checkbox("anim ssbo bind", &component->SPW_AnimSSBO->bBinding);

				ImGui::Checkbox("anim ssbo Initialized", &component->SPW_AnimSSBO->bInitialized);

				ImGui::EndChild();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();

	}

	void ImGuiInspectorPanel::DrawCameraComponent(CameraComponent* component) const
	{
		ImGui::PushID("Camera");

		const char* items[] = {"Perspective", "Ortho", "UIOrtho"};

		if (ImGui::TreeNode(ICON_FA_CAMERA"		 Camera")) 
		{
			if (ImGui::Button("delete"))
			{
				m_Entity->remove<CameraComponent>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			int curr_cameraType = static_cast<int>(component->getType());

			if (ImGui::BeginCombo(" ", items[curr_cameraType]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(items); i++)
				{
					bool isSelected = (curr_cameraType == i);
					if (ImGui::Selectable(items[i], isSelected))
					{
						curr_cameraType = i;
						component->SetCameraType(static_cast<CameraType>(curr_cameraType));
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}


			if (ImGui::BeginChild("Camera", ImVec2(0, 120), true))
			{
				if (component->getType() == CameraType::PerspectiveType)
				{
					// draw component properties
					ImGui::DragFloat("FOV", &component->fov);
					ImGui::DragFloat("Aspect", &component->aspect);
					ImGui::DragFloat("Near", &component->near);
					ImGui::DragFloat("Far", &component->far);
				}
				else if (component->getType() == CameraType::OrthoType)
				{
					// draw component properties
					ImGui::DragFloat("Left", &component->left);
					ImGui::DragFloat("Right", &component->right);
					ImGui::DragFloat("Bottom", &component->bottom);
					ImGui::DragFloat("Top", &component->top);
				}
				ImGui::EndChild();

				ImGui::Checkbox("active", &component->whetherMainCam);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	void ImGuiInspectorPanel::DrawLightComponent() const
	{
		ImGui::PushID("Light");
		//ImGui::Image(reinterpret_cast<void*>(m_IconManager->GetLibIcon("file")), k_DefalutImageSize);
		//ImGui::SameLine();

		//  PointLightComponent = 0, DirectionalLightComponent = 1;
		const char* items[] = {"Point Light","Directional Light"};
		// ImGui::LabelText();
		if (ImGui::TreeNode(ICON_FA_LIGHTBULB"		Light Source"))
		{
			int orignal_lightType = m_Entity->has<PointLightComponent>() ? 0 : 1;

			if (ImGui::Button("delete"))
			{
				if (orignal_lightType == 0)
					m_Entity->remove<PointLightComponent>();
				else
					m_Entity->remove<DirectionalLightComponent>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			int current_lightType = orignal_lightType;
			ImGui::Text("Light Type: ");
			ImGui::SameLine();

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

			if (current_lightType != orignal_lightType)
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

			if (m_Entity->has<PointLightComponent>())
			{
				auto&& pointlight_component = m_Entity->component<PointLightComponent>();
				if (ImGui::BeginChild("Point Light", ImVec2(0, 180), true))
				{
					// draw component properties
					ImGui::DragFloat("Constant", &pointlight_component->constant);
					ImGui::DragFloat("Linear", &pointlight_component->linear);
					ImGui::DragFloat("Quadratic", &pointlight_component->quadratic);
					ImGui::DragFloat3("Ambient", glm::value_ptr(pointlight_component->ambient));
					ImGui::DragFloat3("Diffuse", glm::value_ptr(pointlight_component->diffuse));
					ImGui::DragFloat3("Specular", glm::value_ptr(pointlight_component->specular));

					ImGui::EndChild();
				}
				ImGui::TreePop();
			}

			if (m_Entity->has<DirectionalLightComponent>())
			{
				auto&& directionallight_component = m_Entity->component<DirectionalLightComponent>();
				if (ImGui::BeginChild("Directional Light", ImVec2(0, 90), true))
				{
					// draw component properties
					ImGui::DragFloat3("Ambient", glm::value_ptr(directionallight_component->ambient));
					ImGui::DragFloat3("Diffuse", glm::value_ptr(directionallight_component->diffuse));
					ImGui::DragFloat3("Specular", glm::value_ptr(directionallight_component->specular));

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
		if (ImGui::TreeNode(ICON_FA_MUSIC"			Audio Source")) /* TODO: add icon*/
		{
			if (ImGui::Button("delete"))
			{
				m_Entity->remove<AudioComponent>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

			int i = 1, j = 1;
			for (auto& a : component->allSounds)
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
		if (ImGui::TreeNode(ICON_FA_HEADPHONES"		Audio")) /* TODO: add icon*/
		{
			if (ImGui::Button("delete"))
			{
				m_Entity->remove<AudioListener>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

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
		if (ImGui::TreeNode(ICON_FA_BULLHORN"		Event System")) /* TODO: add icon*/
		{
			if (ImGui::Button("delete"))
			{
				m_Entity->remove<KeyComponent>();

				ImGui::TreePop();
				ImGui::PopID();
				return;
			}

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
