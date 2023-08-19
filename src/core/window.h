#pragma once

#include "renderer.h"

#include <cstddef>

class GLFWwindow;

namespace core {

class Window {
    Window(size_t width, size_t height, GLFWwindow * window);

    Window(Window const &) = delete;
    Window(Window &&) = delete;
    
    Window & operator = (Window const &) = delete;
    Window & operator = (Window &&) = delete;
public:
    static Window & create(size_t width, size_t height);
    static Window & get();

    ~Window();

    void render(Renderer & renderer);
    void keyAction(KeyAction action, Key key);

    const size_t width;
    const size_t height;
private:
    GLFWwindow * window;
    Renderer * executing_renderer = nullptr;
};

} // namespace core
