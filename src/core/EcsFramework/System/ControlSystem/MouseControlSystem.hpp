//
// Created by 29134 on 2023/2/26.
//

#include "Control/MouseEvent.hpp"
#include "EcsFramework/System/SystemI.h"

class MouseControlSystem : public SPW::MouseEventResponder, public SPW::SystemI{

public:
    bool onMouseDown(SPW::MouseEvent *e) override{

        return false;
    }

};