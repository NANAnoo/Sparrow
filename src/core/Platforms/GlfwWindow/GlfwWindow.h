//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_GLFWWINDOW_H
#define SPARROW_GLFWWINDOW_H

#include "ApplicationFramework/WindowI/WindowI.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SPW {
    class GlfwWindow : public WindowI{
    public:
        explicit GlfwWindow() = default;
        GlfwWindow(const GlfwWindow &other);
        ~GlfwWindow();
        void init(WindowMeta meta) override;
        void onUpdate() override;
        const char *title() override {return data.title;}
        void setTitle(const char *t) override {
            data.title = t;
            if (window) {
                glfwSetWindowTitle(window, t);
            }
        }
        int width() override {return data.width;}
        int height() override {return data.height;}
        void setSize(int w, int h) override {
            data.width = w;
            data.height = h;
            if (window) {
                glfwSetWindowSize(window, w, h);
            }
        }
        void stop();
        std::shared_ptr<GraphicsContext> getGraphicsContext() override {
            return graphicsContext;
        }
        void onWindowCreated(const std::function<void(GLFWwindow *handle)> &callback) {
            windowCreatedCallback = callback;
        }
        std::shared_ptr<GraphicsContext> graphicsContext;
    private:
        GLFWwindow *window = nullptr;
        WindowMeta data = {"", 0, 0, nullptr};

        std::function<void(GLFWwindow *handle)> windowCreatedCallback;
    };
}

#endif //SPARROW_GLFWWINDOW_H
