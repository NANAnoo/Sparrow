//
// Created by NANAnoo on 2/11/2023.
// Define Event type and categories
//

#ifndef SPARROW_EVENTDEFINES_H
#define SPARROW_EVENTDEFINES_H

namespace SPW {
    enum EventCategory {
        UnknownCategory = 0,
        MouseCategory = 1 << 1,
        KeyCategory = 1 << 2,
        WindowCategory = 1 << 3,
        AllCategory = MouseCategory | KeyCategory | WindowCategory,
    };
    enum EventType {
        UnknownType = 0,
        WindowResizeType,
        WindowCloseType,
        WindowFrameResizeType,
        KeyDownType,
        KeyHeldType,
        KeyReleasedType,
        KeyInputType,
        MouseDownType,
        MouseHeldType,
        MouseReleasedType,
        CursorMovementType,
        MouseScrollType
    };

    constexpr const char *toString(EventType type) {
        switch (type) {
            case UnknownType: return "Event";
            case WindowResizeType: return "WindowResizeEvent";
            case WindowCloseType: return "WindowCloseEvent";
            case WindowFrameResizeType: return "WindowFrameResizeEvent";
            case KeyDownType: return "KeyDownEvent";
            case KeyHeldType: return "KeyHeldEvent";
            case KeyReleasedType: return "KeyReleasedEvent";
            case KeyInputType: return "KeyInputEvent";
            case MouseDownType: return "MouseDownEvent";
            case MouseHeldType: return "MouseHeldEvent";
            case MouseReleasedType: return "MouseReleasedEvent";
            case CursorMovementType: return "CursorMovementEvent";
            case MouseScrollType: return "MouseScrollEvent";
        }
        return "UnKnownEvent";
    }
}

#endif //SPARROW_EVENTDEFINES_H
