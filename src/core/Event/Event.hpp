//
// Created by NANAnoo on 2/11/2023.
// define event structure , producer and consumer interface
//

#ifndef SPARROW_EVENT_HPP
#define SPARROW_EVENT_HPP

#include "Utils/MacroUtils.h"
#include "EventDefines.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace SPW {
    class Event {
    public:
        using EventHanlder = std::function<void(const Event &e)>;
        explicit Event()= default;

        EventType type = UnknownType;
        EventValue value = UnknownValue;

        bool inKindOf(EventType which) {
            return (which & type) > 0;
        }

        std::shared_ptr<void *> userData;
        bool isConsumed = false;
DEBUG_PROPERTY(std::vector<std::string> processChain = {})
    };
}

#endif //SPARROW_EVENT_HPP
