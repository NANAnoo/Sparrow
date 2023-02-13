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
        KeyCategory = 2 << 1,
    };
    enum EventType {
        UnknownType = 0,
    };
}

#endif //SPARROW_EVENTDEFINES_H
