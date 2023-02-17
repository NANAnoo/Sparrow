#include <iostream>

#include <sol/sol.hpp>

#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Utils/MessageDefines.h"

#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Control/KeyEvent.hpp"
#include "Control/MouseEvent.hpp"
#include "EcsFramework/Scene.hpp"
#include "EcsFramework/Entity/Entity.hpp"

#include "EcsFramework/Component/BasicComponent/NameComponent.h"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "Utils/UUID.hpp"

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";
class SimpleRender {
public:
    SimpleRender();
    void render() const;
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
};

class WOC :
        public SPW::WindowEventResponder,
        public SPW::KeyEventResponder,
        public SPW::MouseEventResponder {
public:
    explicit WOC(const std::shared_ptr<SPW::EventResponderI> &parent, const char *name):
            SPW::WindowEventResponder(parent),
            SPW::KeyEventResponder(parent),
            SPW::MouseEventResponder(parent),
            _name(name){
        }
    explicit WOC(const std::shared_ptr<WOC> &parent, const char *name):
            SPW::WindowEventResponder(std::shared_ptr<SPW::WindowEventResponder>(parent)),
            SPW::KeyEventResponder(std::shared_ptr<SPW::KeyEventResponder>(parent)),
            SPW::MouseEventResponder(std::shared_ptr<SPW::MouseEventResponder>(parent)),
            _name(name){
    }
    bool onKeyDown(SPW::KeyEvent *e) override {
        if (_name[0] == 'C') {
            std::cout << "onKeyDown" << std::endl;
            return true;
        }
        return false;
    }

    bool onMouseDown(SPW::MouseEvent *e) override {
        if (_name[0] == 'B') {
            std::cout << "onMouseDown" << std::endl;
            return true;
        }
        return false;
    }
    bool canRespondTo(const std::shared_ptr<SPW::EventI> &e) final {
        return _name[0] != 'E' || e->category() == SPW::MouseCategory;
    }
    const char *_name;
    const char *getName() final {return _name;}
};

// test usage
class Transformer :
        public SPW::WindowEventResponder {
public:
    explicit Transformer (const std::shared_ptr<SPW::EventResponderI> &parent)
            : SPW::WindowEventResponder(parent) {
    }

    bool onWindowResize(int w, int h) override {
        std::cout << "window resize" << "(" << w << ", " << h << ")" <<std::endl;
        width = w;
        height = h;
        // set projection
        return true;
    }

    // windows size is not equal to frame buffer size
    bool onFrameResize(int w, int h) override {
        glViewport(0, 0, w, h);
        std::cout << "frame resize" << "(" << w << ", " << h << ")" <<std::endl;
        // set projection
        return false;
    }

    int width = -1;
    int height = -1;
    const char *getName() final {
        return "Transformer";
    }
};

class TestDelegate : public SPW::AppDelegateI {
public:
    explicit TestDelegate(std::shared_ptr<SPW::EventResponderI> &app, const char *name) :
            SPW::AppDelegateI(app), _name(name) {
    }
    void onAppInit() final {
        app->window = std::make_shared<SPW::GlfwWindow>();
        app->window->setSize(800, 600);
        app->window->setTitle("SPWTestApp");

        transformer = std::make_shared<Transformer>(app->delegate.lock());
        transformer->width = app->window->width();
        transformer->height = app->window->height();
        SPW::OBSERVE_MSG_ONCE(SPW::kMsgApplicationInited, [this](SPW::Message msg) {
            this->render = std::make_shared<SimpleRender>();
        })

        auto A = std::make_shared<WOC>(app->delegate.lock(), "A");
        auto B = std::make_shared<WOC>(A, "B");
        auto C = std::make_shared<WOC>(B, "C");
        auto D = std::make_shared<WOC>(B, "D");
        auto E = std::make_shared<WOC>(A, "E");
        auto F = std::make_shared<WOC>(E, "F");
        auto G = std::make_shared<WOC>(E, "G");
        app->postEvent(std::make_shared<SPW::KeyEvent>(SPW::KeyDownType));
    }
    void beforeAppUpdate() final{
        app->postEvent(std::make_shared<SPW::MouseEvent>(SPW::MouseDownType));

        // ECS test
        scene = SPW::Scene::create(app->delegate.lock());
        scene->createEntity("Game Object 1");
        scene->createEntity("Game Object 2");
        scene->createEntity("Game Object 3");

        std::cout << "------------------------" << std::endl;
        std::cout << "Print ID and names 1" << std::endl;
        scene->forEachEntity<SPW::IDComponent, SPW::NameComponent>(
            [](const SPW::Entity &e) {
                auto [id, name] =
                    e.combined<SPW::IDComponent, SPW::NameComponent>();
                std::cout << id->getID().toString() << std::endl;
                std::cout << name->getName() << std::endl;
        });

        std::cout << "------------------------" << std::endl;
        std::cout << "Print ID and names 2" << std::endl;
        scene->forEach([](const SPW::IDComponent &id, const SPW::NameComponent &name) {
            std::cout << id.getID().toString() << std::endl;
            std::cout << name.getName() << std::endl;
        }, SPW::IDComponent, SPW::NameComponent);

        std::cout << "------------------------" << std::endl;
        std::cout << "Print Names" << std::endl;
        scene->forEach([](const SPW::NameComponent &name) {
            std::cout << name.getName() << std::endl;
        }, SPW::NameComponent);

        std::cout << "------------------------" << std::endl;
        std::cout << "Print ID" << std::endl;
        scene->forEach([](const SPW::IDComponent &id) {
            std::cout << id.getID().toString() << std::endl;
        }, SPW::IDComponent);
    }

    void onAppUpdate(const SPW::TimeDuration &du) final{
        // physics, computation
    }

    void afterAppUpdate() final{
        render->render();
    }
    void onUnConsumedEvents(std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        for (auto &e : events) {
            DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        }
    }
    void onAppStopped() final{
        sol::state state;
        state.open_libraries(sol::lib::base);
        try {
            if(state.script_file("./resources/scripts/lua/test.lua").valid()) {
                sol::protected_function main_function=state["main"];
                sol::protected_function_result result=main_function();
                if (!result.valid()) {
                    std::cout << "execution error" << std::endl;
                }
            }
        } catch (sol::error &e) {
            std::cout << e.what() << std::endl;
        }
        std::cout << "app stopped" << std::endl;
    }

    void solveEvent(const std::shared_ptr<SPW::EventI> &e) final {
        e->dispatch<SPW::WindowCloseType, SPW::WindowEvent>(
                [this](SPW::WindowEvent *e){
                    // close application
                    app->stop();
                    return true;
                });
        SPW::EventResponderI::solveEvent(e);
    }

    const char *getName() final {return _name;}
    const char *_name;
    std::shared_ptr<Transformer> transformer;
    std::shared_ptr<SimpleRender> render;
    std::shared_ptr<SPW::Scene> scene;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto appProxy =
        SPW::Application::create<TestDelegate>(
            "SPWTestApp"
            );

    return appProxy->app->run(argc, argv);
}

SimpleRender::SimpleRender() {
    // Compile shaders
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    float vertices[] = {
            0.0f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void SimpleRender::render() const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
