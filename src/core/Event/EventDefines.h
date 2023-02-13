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
        WindowCategory = 1 << 3
    };
    enum EventType {
        UnknownType = 0,
        WindowResizeType,
    };
}

#endif //SPARROW_EVENTDEFINES_H
