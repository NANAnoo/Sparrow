/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include <string>
#include <vector>
#include <functional>
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

	enum class UIComponentType
	{
		Panel,
		Dockspace,
		MenuBar,
		ObjectPanel,
		HierarchyPanel,
		InspectorPanel,
		Axis,
		Image,
		FileExplorer
	};
}