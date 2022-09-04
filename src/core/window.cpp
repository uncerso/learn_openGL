#include "window.h"

#include "opengl.h"
#include "exception.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace core {

namespace {

std::unique_ptr<Window> windowSingleton;

void keyCallback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mode */) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q))
        glfwSetWindowShouldClose(window, GL_TRUE);
}

} // namespace

Window& Window::create(size_t width, size_t height) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    auto* window = glfwCreateWindow(static_cast<GLint>(width), static_cast<GLint>(height), "LearnOpenGL", nullptr, nullptr);
    REQUIRE(window, "Failed to create GLFW window");

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    glewExperimental = GL_TRUE;
    REQUIRE(glewInit() == GLEW_OK, "Failed to initialize GLEW");

    windowSingleton.reset(new Window(width, height, window));
    return *windowSingleton;
}

Window& Window::get() {
    REQUIRE(windowSingleton, "You have to create window before 'Window::get' can be called");
    return *windowSingleton;
}


Window::Window(size_t width, size_t height, GLFWwindow * window)
    : width(width)
    , height(height)
    , window(window)
{}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Window::render(Renderer & renderer) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);  
    glViewport(0, 0, w, h);

    renderer.prepare();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.render();

        glfwSwapBuffers(window);
    }
}

} // namespace core
