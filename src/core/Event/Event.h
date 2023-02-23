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
    class Application;
    class EventResponderI;
#define EVENT_RESPONDER(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

    class EventI {
    public:
        EventI() = default;
        EventI(const EventI& other) {
            consumed = other.consumed;
        }
        virtual ~EventI() = default;
        using EventHanlder = std::function<void(const std::shared_ptr<EventI> &e)>;
        virtual EventType type() {return UnknownType;}
        virtual EventCategory category() {return UnknownCategory;}
        bool isIn(EventCategory c) {
            return (category() & c) > 0;
        }
        const char *name() {
            return toString(this->type());
        }

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
    private:
        bool consumed = false;
        DEBUG_PROPERTY(std::vector<std::string> processChain = {})
        friend EventResponderI;
        friend Application;
        friend std::ostream &operator<<(std::ostream &os, EventI *e);
    };

    class EventResponderI {
    public:
        EventResponderI() = default;
        explicit EventResponderI(const std::shared_ptr<EventResponderI> &p) {
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
        virtual const char* getName() {
            return "Unknown";
        }
        virtual bool canRespondTo(const std::shared_ptr<EventI> &e) {
            return true;
        }
        virtual void solveEvent(const std::shared_ptr<EventI> &e) {}

        virtual EventCategory listeningCategory() {return AllCategory;}
    private:
        std::unordered_set<EventResponderI*> subResponders;
        std::weak_ptr<EventResponderI> parent;

        // private logic
        void onEvent(const std::shared_ptr<EventI> &e) {
            if (e->consumed || !canRespondTo(e)) return;
            // pass event to children nodes
            for (auto sub : subResponders) {
                if (e->isIn(sub->listeningCategory()))
                    sub->onEvent(e);
            }
            // try to consume event
            if (!e->consumed)
                dispatchEvent(e);
        }
        void dispatchEvent(const std::shared_ptr<EventI> &e) {
            // track event processing chain in debug mode
            DEBUG_EXPRESSION(e->processChain.emplace_back(getName());)
            // try to consume event
            solveEvent(e);
            DEBUG_EXPRESSION(if (e->consumed)std::cout << e << std::endl;)
        }
    friend Application;
    };
}

#endif //SPARROW_EVENT_H
