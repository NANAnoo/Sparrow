//
// Created by NANAnoo on 2/11/2023.
// define event structure , producer and consumer interface
//

#ifndef SPARROW_EVENT_HPP
#define SPARROW_EVENT_HPP

#include "Utils/MacroUtils.h"
#include "EventDefines.h"
#include "Utils/SingletonBase.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace SPW {

    class EventI {
    public:
        using EventHanlder = std::function<void(EventI &e)>;
        virtual EventType type() {return UnknownType;}
        virtual EventCategory category() {return UnknownCategory;}
        bool isIn(EventCategory c) {
            return (category() & c) > 0;
        }

        DEBUG_PROPERTY(std::vector<std::string> processChain = {})
        bool consumed = false;
    };

    class EventResponderI {
    public:
        EventResponderI() = default;
        explicit EventResponderI(std::shared_ptr<EventResponderI> &p) {
            if (p) {
                p->subResponders.insert(this);
                parent = p;
            }
        }
        ~EventResponderI() {
            if (!parent.expired()) {
                auto strong_p = parent.lock();
                strong_p->subResponders.erase(this);
            }
        }
        virtual const char* getName() {return "Unknown";}
        virtual void onEvent(EventI *e) {
            // use for debugging
            DEBUG_EXPRESSION(e->processChain.emplace_back(getName());)

            for (auto &sub : subResponders) {
                // pass through event
                if (!e->consumed && e->isIn(sub->listeningCategory())) {
                    sub->onEvent(e);
                }
            }

        }

        virtual EventCategory listeningCategory() {return UnknownCategory;}
    private:
        std::unordered_set<EventResponderI*> subResponders;
        std::weak_ptr<EventResponderI> parent;
    };
}

#endif //SPARROW_EVENT_HPP
