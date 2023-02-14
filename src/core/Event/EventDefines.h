//
// Created by NANAnoo on 2/11/2023.
// Define Event type and categories
//

#ifndef SPARROW_EVENTDEFINES_H
#define SPARROW_EVENTDEFINES_H

namespace SPW {
    enum EventCategory {
        UnknownCategory = 0,
        MouthCategory = 1 << 1,
        KeyCategory = 1 << 2,
        WindowCategory = 1 << 3,
        AllCategory = MouthCategory | KeyCategory | WindowCategory,
    };
    enum EventType {
        UnknownType = 0,
        WindowResizeType,
        WindowCloseType,
        WindowFrameResizeType,
        KeyDownType,
        MouthDownType
    };

    constexpr const char *toString(EventType type) {
        switch (type) {
            case UnknownType: return "Event";
            case WindowResizeType: return "WindowResizeEvent";
            case WindowCloseType: return "WindowCloseEvent";
            case WindowFrameResizeType: return "WindowFrameResizeEvent";
            case KeyDownType: return "KeyDownEvent";
            case MouthDownType: return "MouthDownType";
        }
    }
}

#endif //SPARROW_EVENTDEFINES_H
