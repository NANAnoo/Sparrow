//
// Created by NANAnoo on 2/11/2023.
//

#include "GlfwWindow.h"

#include <unordered_map>
#include <iostream>

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
            glfwSetErrorCallback(glfw_error_callback);
        }
        window = glfwCreateWindow(meta.width, meta.height, meta.title, nullptr, nullptr);

        if (window) {
            all_windows[window] = this;
            {
                // TODO, move this block to render module
                glfwMakeContextCurrent(window);
                gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            }

        } else {
            std::cout << "Window create failed !" << std::endl;
        }
        // set up event callbacks

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
        for (auto &pair : all_windows) {
            if (pair.second == this) {
                all_windows.erase(pair.first);
            }
        }
        stop();
    }
}