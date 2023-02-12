//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_WINDOWI_H
#define SPARROW_WINDOWI_H

#include "Event/Event.hpp"

namespace SPW {
    struct WindowMeta {
        const char *title;
        int width;
        int height;
        EventI::EventHanlder handler;
    };
    class WindowI {
    public:
        virtual void init(WindowMeta meta) = 0;
        virtual void onUpdate() = 0;
        virtual const char *title() = 0;
        virtual void setTitle(const char *t) = 0;
        virtual int width() = 0;
        virtual int height() = 0;
        virtual void setSize(int w, int h) = 0;
    };
}

#endif //SPARROW_WINDOWI_H
