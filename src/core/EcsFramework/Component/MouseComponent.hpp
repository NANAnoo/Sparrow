//
// Created by 29134 on 2023/2/26.
//

#include "ComponentI.h"
#include "EcsFramework/Entity/Entity.hpp"

namespace SPW {

    class MouseComponent : public ComponentI {
    public:
        std::function<void(const SPW::Entity& e, int button_code)> onMouseDownCallBack;
        std::function<void(const SPW::Entity& e, int button_code)> onMouseHeldCallBack;
        std::function<void(const SPW::Entity& e, int button_code)> onMouseReleasedCallBack;
        std::function<void(const SPW::Entity& e, double scroll_offset)> onMouseScrollCallBack;
        std::function<void(const SPW::Entity& e, double cursor_x, double cursor_y, double cursor_X_bias, double cursor_Y_bias)> cursorMovementCallBack;
    };

}