/*
 * @date   2023-04-15
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

namespace SPW
{
	enum class ComponentType
	{
		/* basic */
		IDComponent,
		NameComponent,

		/* essential */
		TransformComponent,
		MeshComponent,

		/* event */
		KeyComponent,
		MouseComponent,

		/* render */
		CameraComponent,
		PointLightComponent,
		DirectionalLightComponent,

		/* audio */
		AudioComponent,
		AudioListener,
	};

	inline std::string ComponentTypeToString(ComponentType componentType) {
		switch (static_cast<int>(componentType)) {
		case static_cast<int>(ComponentType::IDComponent):
			return "IDComponent";
		case static_cast<int>(ComponentType::NameComponent):
			return "NameComponent";
		case static_cast<int>(ComponentType::TransformComponent):
			return "TransformComponent";
		case static_cast<int>(ComponentType::MeshComponent):
			return "MeshComponent";
		case static_cast<int>(ComponentType::KeyComponent):
			return "KeyComponent";
		case static_cast<int>(ComponentType::MouseComponent):
			return "MouseComponent";
		case static_cast<int>(ComponentType::PointLightComponent):
			return "PointLightComponent";
		case static_cast<int>(ComponentType::DirectionalLightComponent):
			return "DirectionalLightComponent";
		case static_cast<int>(ComponentType::AudioComponent):
			return "AudioComponent";
		case static_cast<int>(ComponentType::AudioListener):
			return "AudioListener";
		default:
			return "Unknown";
		}
	}

}