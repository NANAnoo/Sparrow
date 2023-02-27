//
// Created by 29134 on 2023/2/26.
//

#include "ComponentI.h"
#include "EcsFramework/Entity/Entity.hpp"

namespace SPW {

    class MouseComponent : public ComponentI {
    public:
        std::function<void(SPW::Entity* e, int button_code)> onMouseDownCallBack;
        std::function<void(SPW::Entity* e, int button_code)> onMouseHeldCallBack;
        std::function<void(SPW::Entity* e, int button_code)> onMouseReleasedCallBack;
        std::function<void(SPW::Entity* e, int button_code, double scroll_offset)> onMouseScrollCallBack;
    };

}
