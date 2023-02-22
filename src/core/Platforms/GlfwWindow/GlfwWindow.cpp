//
// Created by NANAnoo on 2/11/2023.
//

#include "GlfwWindow.h"

#include <unordered_map>
#include <iostream>
#include "ApplicationFramework/WindowI/WindowEvent.h"

namespace SPW {
    // pass callback from glfw window to GLWindow
    static std::unordered_map<GLFWwindow *, GlfwWindow *> all_windows;
    static void glfw_error_callback(int error, const char *errMsg) {
        std::cout << "Error code " << error << "Description " << errMsg << std::endl;
    }
    GlfwWindow::GlfwWindow(const GlfwWindow &other) {
        std::cout << "Test" << std::endl;
    }
    void GlfwWindow::init(SPW::WindowMeta meta) {
        data = meta;
        if (all_windows.empty()) {
            if (int success = glfwInit(); GLFW_TRUE != success) {
                std::cout << "GLFW init failed !" << std::endl;
            }
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwSetErrorCallback(glfw_error_callback);
        }
        window = glfwCreateWindow(meta.width, meta.height, meta.title, nullptr, nullptr);

        if (window) {
            all_windows[window] = this;
            if (windowCreatedCallback) {
                windowCreatedCallback(window);
            }
        } else {
            std::cout << "Window create failed !" << std::endl;
        }
        // set up event callbacks
        glfwSetWindowCloseCallback(window, [](GLFWwindow *win) {
            auto realWindow = all_windows[win];
            realWindow->data.handler(std::make_shared<WindowEvent>(
                    WindowCloseType,
                    realWindow->width(),
                    realWindow->height()));
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow *win, int w, int h) {
            auto realWindow = all_windows[win];
            realWindow->data.width = w;
            realWindow->data.height = h;
            realWindow->data.handler(std::make_shared<WindowEvent>(
                    WindowResizeType,
                    realWindow->width(),
                    realWindow->height()));
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *win, int w, int h) {
            auto realWindow = all_windows[win];
            realWindow->data.handler(std::make_shared<WindowEvent>(
                    WindowFrameResizeType, w, h));
        });



        glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {

        });
    }

    void GlfwWindow::stop() {
        all_windows.erase(window);
        glfwDestroyWindow(window);
        if (all_windows.empty())
        {
            glfwTerminate();
        }
    }

    void GlfwWindow::onUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    GlfwWindow::~GlfwWindow() {
        stop();
    }
}