//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_WINDOWI_H
#define SPARROW_WINDOWI_H

#include "Event/Event.h"
#include "Render/GraphicsContext.h"
#include "Control/KeyCodes.h"


namespace SPW {
    struct WindowMeta {
        std::string title;
        int width;
        int height;
        EventI::EventHanlder handler;
    };
    class WindowI {
    public:
        virtual void init(WindowMeta meta) = 0;
        virtual void onUpdate() = 0;
        virtual const char *title() = 0;
        virtual void setTitle(std::string t) = 0;
        virtual int width() = 0;
        virtual int height() = 0;
        virtual int frameWidth() = 0;
        virtual int frameHeight() = 0;
        virtual void setSize(int w, int h) = 0;
        virtual std::shared_ptr<GraphicsContext> getGraphicsContext() = 0;
        virtual void enableCursor(bool enabled) = 0;
    };
}

#endif //SPARROW_WINDOWI_H
