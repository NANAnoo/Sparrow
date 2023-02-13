//
// Created by NANAnoo on 2/11/2023.
// define event structure , producer and consumer interface
//

#ifndef SPARROW_EVENT_H
#define SPARROW_EVENT_H

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

#define EVENT_RESPONDER(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    class EventI {
    public:
        using EventHanlder = std::function<void(const std::shared_ptr<EventI> &e)>;
        virtual EventType type() {return UnknownType;}
        virtual EventCategory category() {return UnknownCategory;}
        bool isIn(EventCategory c) {
            return (category() & c) > 0;
        }
        const char *name() {
            return toString(this->type());
        }

        DEBUG_PROPERTY(std::vector<std::string> processChain = {})
        bool consumed = false;

        template<typename T>
        using EventFunc = std::function<bool(T *)>;

        // dispatcher
        template<EventType type, typename T>
        void dispatch(EventFunc<T> func) {
            if (!this->consumed && this->type() == type) {
                T *te = (T *)this;
                this->consumed = func(te);
            }
        }
        friend std::ostream &operator<<(std::ostream &os, EventI *e);
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
        virtual void onEvent(const std::shared_ptr<EventI> &e) {
            // use for debugging
            DEBUG_EXPRESSION(e->processChain.emplace_back(getName());)

            for (auto &sub : subResponders) {
                // pass through event
                if (!e->consumed && e->isIn(sub->listeningCategory())) {
                    sub->onEvent(e);
                    DEBUG_EXPRESSION(if (e->consumed) {std::cout << e << std::endl;})
                }
            }

        }

        virtual EventCategory listeningCategory() {return UnknownCategory;}
    private:
        std::unordered_set<EventResponderI*> subResponders;
        std::weak_ptr<EventResponderI> parent;
    };
}

#endif //SPARROW_EVENT_H
