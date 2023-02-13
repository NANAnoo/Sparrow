//
// Created by ZhangHao on 2023/2/13.
//

#include "Event.h"

namespace SPW {
    std::ostream &operator<<(std::ostream &os, EventI *e) {
        std::cout << "[" << e->name() << "]:";
        DEBUG_EXPRESSION(
        for (auto &name:e->processChain) {
            std::cout << name << "->";
        }
        std::cout << (e->consumed ? "[consumed]" : "[not consumed]");
        )
        return os;
    }
}
