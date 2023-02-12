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
    class EventI {
    public:
        using EventHanlder = std::function<void(const EventI &e)>;
        virtual EventType type() {return UnknownType;}
        virtual EventCategory category() {return UnknownCategory;}
        bool isIn(EventCategory c) {
            return (type() & c) > 0;
        }
        bool consumed = false;
DEBUG_PROPERTY(std::vector<std::string> processChain = {})
    };
}

#endif //SPARROW_EVENT_HPP
