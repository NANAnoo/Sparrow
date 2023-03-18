//
// Created by NANAnoo on 2/11/2023.
//

#include "GlfwWindow.h"

#include <unordered_map>
#include <iostream>
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyEvent.hpp"
#include "Control/MouseCodes.h"
#include "Control/MouseEvent.hpp"

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
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwSetErrorCallback(glfw_error_callback);
        }
        window = glfwCreateWindow(meta.width, meta.height, meta.title, nullptr, nullptr);

        if (window) {
            glfwGetFramebufferSize(window, &fWidth, &fHeight);
            all_windows[window] = this;
            if (windowCreatedCallback) {
                windowCreatedCallback(window);
            }
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
            realWindow->fWidth = w;
            realWindow->fHeight = h;
            realWindow->data.handler(std::make_shared<WindowEvent>(
                    WindowFrameResizeType, w, h));
        });

        glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
            auto realWindow = all_windows[win];
            auto keyCode = static_cast<KeyCode>(key);
            if(action == GLFW_RELEASE){
                realWindow->downKeys.erase(key);
                realWindow->data.handler(std::make_shared<KeyEvent>(
                    KeyReleasedType, keyCode));
            }
            else if(action == GLFW_PRESS){
                realWindow->downKeys.insert(key);
                realWindow->data.handler(std::make_shared<KeyEvent>(
                        KeyDownType, keyCode));
            }
        });


        glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods){

            auto realWindow = all_windows[win];
            auto mouseCode = static_cast<MouseCode>(button);
            if(action == GLFW_PRESS){
                realWindow->data.handler(std::make_shared<MouseEvent>(
                        MouseDownType, mouseCode, 0));
            }
            else if(action == GLFW_RELEASE){
                realWindow->data.handler(std::make_shared<MouseEvent>(
                        MouseReleasedType, mouseCode, 0));
            }
            else if(action == GLFW_REPEAT){
                realWindow->data.handler(std::make_shared<MouseEvent>(
                        MouseHeldType, mouseCode, 0));
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* win, double x_offset, double y_offset){
            auto realWindow = all_windows[win];
            realWindow->data.handler(std::make_shared<MouseEvent>(
                    MouseScrollType, MouseCode::ButtonMiddle, y_offset));
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

    double sCursorLastX_Pos = 0;
    double sCursorLastY_Pos = 0;
    double sCursorX_PosBias = 0;
    double sCursorY_PosBias = 0;
    void GlfwWindow::onUpdate() {
        glfwPollEvents();
        double xpos = 0, ypos = 0;
        glfwGetCursorPos(window, &xpos, &ypos);
        sCursorX_PosBias = xpos - sCursorLastX_Pos;
        sCursorY_PosBias = ypos - sCursorLastY_Pos;
        sCursorLastX_Pos = xpos;
        sCursorLastY_Pos = ypos;
        auto cursor_e = std::make_shared<MouseEvent>(CursorMovementType, xpos, ypos, sCursorX_PosBias , sCursorY_PosBias);
        data.handler(cursor_e);
        for (auto &key : downKeys) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                data.handler(std::make_shared<KeyEvent>(
                        KeyHeldType, static_cast<KeyCode>(key)));
            }
        }
    }

    GlfwWindow::~GlfwWindow() {
        stop();
    }
}