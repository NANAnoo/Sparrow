#include <iostream>
#include "SparrowCore.h"
#include "Platforms/GlfwWindow/GlfwWindow.h"
#include "Utils/MessageDefines.h"

#include "Event/WindowEvent.h"

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

// test usage
class Transformer :
        public SPW::WindowEventResponder{
public:
    explicit Transformer (std::shared_ptr<SPW::EventResponderI> &parent)
            : SPW::WindowEventResponder(parent){
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
    const char *getName() final {return "Transformer";}
};

class TestDelegate : public SPW::AppDelegateI {
    void onAppInit(std::shared_ptr<SPW::Application> app) final {
        app->window = std::make_shared<SPW::GlfwWindow>();
        app->window->setSize(800, 600);
        app->window->setTitle("SPWTestApp");
        auto ptr = std::shared_ptr<SPW::EventResponderI>(app->weakThis);
        transformer = std::make_shared<Transformer>(ptr);
        transformer->width = app->window->width();
        transformer->height = app->window->height();
        SPW::OBSERVE_MSG_ONCE(SPW::kMsgApplicationInited, [this](SPW::Message msg) {
            this->render = std::make_shared<SimpleRender>();
        })
    }
    void beforeAppUpdate(std::shared_ptr<SPW::Application> app) final{
        if (transformer->width < 500) {
            transformer->width = 500;
        }
        if (transformer->height < 500) {
            transformer->height = 400;
        }
        app->window->setSize(transformer->width, transformer->height);
    }
    void onAppUpdate(std::shared_ptr<SPW::Application> app, const SPW::TimeDuration &du) final{
        render->render();
    }

    void afterAppUpdate(std::shared_ptr<SPW::Application> app) final{

    }
    void onUnConsumedEvents(std::shared_ptr<SPW::Application> app, std::vector<std::shared_ptr<SPW::EventI>> &events) final{
        for (auto &e : events) {
            DEBUG_EXPRESSION(std::cout << e.get() << std::endl;)
        }
    }
    void onAppStopped(std::shared_ptr<SPW::Application> app) final{
        std::cout << "app stopped" << std::endl;
    }
    void onAppDestroy() final{
        std::cout << "app destroyed" << std::endl;
    }

    std::shared_ptr<Transformer> transformer;
    std::shared_ptr<SimpleRender> render;
};

// main entrance
int main(int argc, char **argv) {
    // app test
    auto delegate = std::shared_ptr<SPW::AppDelegateI>(new TestDelegate());
    auto app = SPW::Application::create(delegate);
    return app->run(argc, argv);
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
