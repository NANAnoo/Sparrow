//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_WINDOWI_H
#define SPARROW_WINDOWI_H

#include "Event/Event.hpp"

namespace SPW {
    struct WindowMeta {
        const char *title;
        unsigned int width;
        unsigned int height;
        Event::EventHanlder handler;
    };
    class WindowI {
        public:
            virtual void init(WindowMeta meta) = 0;
            virtual void onUpdate() = 0;
            virtual void setSize(unsigned int w, unsigned int h) = 0;
        private:
            WindowMeta data;
    };
}

#endif //SPARROW_WINDOWI_H
