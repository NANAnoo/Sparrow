/*
 * @date   2023-04-11
 * @author dudu
 * @brief  https://github.com/leiradel/ImGuiAl/
 */
#pragma once

#include "ImGui/ImGuiDefinitions.h"

namespace SPW {

    class ImGuiMessageBox
    {
    public:
        ImGuiMessageBox(const char* title, const char* icon, const char* text, std::vector<const char*>&& captions, bool show_checkbox = false)
	        : m_Title(title)
    		, m_Icon()
    		, m_Text(text)
    		, m_Captions(captions)
    		, m_ShowCheckbox(show_checkbox)
    		, m_DontAskAgain(false)
    		, m_Selected(0)
        {   }

        ~ImGuiMessageBox() = default;

        void SetTrigger() const { ImGui::OpenPopup(m_Title); }
        int  Exec();
    	void AskAgain();

    protected:
        const char* m_Title;
        const char* m_Icon;
        const char* m_Text;
        std::vector<const char*> m_Captions;
        bool m_ShowCheckbox;
        bool m_DontAskAgain;
        uint32_t m_Selected;
        uint32_t min_width  = 200;
        uint32_t min_height = 100;

    public:
        bool trigger_flag = false;
    };

}  // namespace SPW

