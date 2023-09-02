#pragma once

#include "renderer.h"

#include <cstddef>
#include <optional>
#include <glm/vec2.hpp>

class GLFWwindow;

namespace core {

class Window {
    Window(size_t width, size_t height, GLFWwindow * window);

    Window(Window const &) = delete;
    Window(Window &&) = delete;
    
    Window & operator = (Window const &) = delete;
    Window & operator = (Window &&) = delete;
public:
    enum class ExitReason {
        Quit, RequestedPrev, RequestedNext
    };

    static Window & create(size_t width, size_t height);
    static Window & get();

    ~Window();

    ExitReason render(Renderer & renderer);
    void keyAction(KeyAction action, Key key);
    void mouseMoveAction(glm::vec2 pos);

    void stopRendering(ExitReason);

    const size_t width;
    const size_t height;
private:
    GLFWwindow * window;
    Renderer * executing_renderer = nullptr;
    std::optional<ExitReason> exit_reason;
    std::optional<glm::vec2> prev_mouse_pos;
};

} // namespace core
