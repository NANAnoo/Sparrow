//
// Created by NANAnoo on 2/11/2023.
//

#include "GlfwWindow.h"

#include <unordered_map>
#include <iostream>


namespace SPW {
    // pass callback from glfw window to GLWindow
    static std::unordered_map<GLFWwindow *, std::weak_ptr<GlfwWindow>> all_windows;
    static void glfw_error_callback(int error, const char *errMsg) {
        std::cout << "Error code " << error << "Description " << errMsg << std::endl;
    }
    void GlfwWindow::init(SPW::WindowMeta meta) {
        data = meta;
        if (all_windows.empty()) {
            if (int success = glfwInit(); GLFW_TRUE == success) {

                } else {
                return;
            }
            glfwSetErrorCallback(glfw_error_callback);
        }
        window = glfwCreateWindow(meta.width, meta.height, meta.title, nullptr, nullptr);

        if (window) {
            all_windows[window] = std::make_shared<GlfwWindow>(*this);
            glfwMakeContextCurrent(window);
        } else {
            std::cout << "Window create failed !" << std::endl;
        }

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

    }

    GlfwWindow::~GlfwWindow() {
        stop();
    }
}