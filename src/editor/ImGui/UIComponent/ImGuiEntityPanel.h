/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiInspectorPanel.h"
#include "ImGuiPanel.h"
#include "Asset/Serializer/EntitySerializer.h"
#include "ImGui/IconsFontAwesome6.h"

namespace SPW
{
	class ImGuiEntityPanel : public ImGuiPanel
	{
	public:
		using MenuItemCallback = std::function<void()>;

		struct MenuItem
		{
			std::string ID;
			std::string name;
			MenuItemCallback callback;
		};

		ImGuiEntityPanel(const std::string& title, bool* open = nullptr)
			: ImGuiPanel(ICON_FA_LIST"  "+ title, open, ImGuiWindowFlags_NoMove )
			, m_FileDialog(std::make_shared<ImGuiFileDialog>())
		{
		}

		void UpdateMenuItemLabel(const std::string& id, const std::string& label);
		void AddMenuItem(const std::string& id, const std::string& label, MenuItemCallback callback);
		void ClearItems() { m_Items.clear(); }
		void RemoveMenuItem(const std::string& ID);
		void SetActiveScene(const std::shared_ptr<Scene>& scene) { m_ScenePtr = scene; }

	protected:
		void Draw() override;

	private:
		std::unordered_map<std::string, MenuItem> m_Items;
		std::shared_ptr<Scene> m_ScenePtr;
		std::shared_ptr<ImGuiFileDialog> m_FileDialog;
	};
}
