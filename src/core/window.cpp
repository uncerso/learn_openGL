#include "window.h"

#include "opengl.h"
#include "exception.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace core {

namespace {

std::unique_ptr<Window> windowSingleton;

Key toKey(int glfw_key);
KeyAction toKeyAction(int glfw_action);

void keyCallback(GLFWwindow* window, int glfw_key, int /* scancode */, int glfw_action, int /* mode */) {
    Key key = toKey(glfw_key);
    KeyAction action = toKeyAction(glfw_action);
    if (action == KeyAction::Press && (key == Key::ESCAPE || key == Key::Q)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if (windowSingleton)
        windowSingleton->keyAction(action, key);
}

void error_callback(int error, char const * msg) {
    using namespace std::string_literals;
    throw " ["s + std::to_string(error) + "] "s + msg;
}

template <typename T>
struct PointerGuard {
    T * & p;
    PointerGuard(T * & p) : p(p) {}
    PointerGuard(T * & p, T * val) : PointerGuard(p) { p = val; }
    ~PointerGuard() { p = nullptr; }
};

} // namespace

Window& Window::create(size_t width, size_t height) {
    glfwSetErrorCallback(error_callback);

    REQUIRE(glfwInit()==GLFW_TRUE, "Cannot init glfw");

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
    PointerGuard _(executing_renderer, &renderer);

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

void Window::keyAction(KeyAction action, Key key) {
    if (executing_renderer)
        executing_renderer->keyAction(action, key);
}

namespace {

KeyAction toKeyAction(int glfw_action) {
    switch (glfw_action) {
    case GLFW_RELEASE : return KeyAction::Release;
    case GLFW_PRESS   : return KeyAction::Press;
    case GLFW_REPEAT  : return KeyAction::Repeat;
    default: throw "unknown key action: " + std::to_string(glfw_action);
    }
}

Key toKey(int glfw_key) {
    switch (glfw_key) {
    /* Printable keys */
    case GLFW_KEY_SPACE         : return Key::SPACE;
    case GLFW_KEY_APOSTROPHE    : return Key::APOSTROPHE;
    case GLFW_KEY_COMMA         : return Key::COMMA;
    case GLFW_KEY_MINUS         : return Key::MINUS;
    case GLFW_KEY_PERIOD        : return Key::PERIOD;
    case GLFW_KEY_SLASH         : return Key::SLASH;
    case GLFW_KEY_0             : return Key::DIGIT_0;
    case GLFW_KEY_1             : return Key::DIGIT_1;
    case GLFW_KEY_2             : return Key::DIGIT_2;
    case GLFW_KEY_3             : return Key::DIGIT_3;
    case GLFW_KEY_4             : return Key::DIGIT_4;
    case GLFW_KEY_5             : return Key::DIGIT_5;
    case GLFW_KEY_6             : return Key::DIGIT_6;
    case GLFW_KEY_7             : return Key::DIGIT_7;
    case GLFW_KEY_8             : return Key::DIGIT_8;
    case GLFW_KEY_9             : return Key::DIGIT_9;
    case GLFW_KEY_SEMICOLON     : return Key::SEMICOLON;
    case GLFW_KEY_EQUAL         : return Key::EQUAL;
    case GLFW_KEY_A             : return Key::A;
    case GLFW_KEY_B             : return Key::B;
    case GLFW_KEY_C             : return Key::C;
    case GLFW_KEY_D             : return Key::D;
    case GLFW_KEY_E             : return Key::E;
    case GLFW_KEY_F             : return Key::F;
    case GLFW_KEY_G             : return Key::G;
    case GLFW_KEY_H             : return Key::H;
    case GLFW_KEY_I             : return Key::I;
    case GLFW_KEY_J             : return Key::J;
    case GLFW_KEY_K             : return Key::K;
    case GLFW_KEY_L             : return Key::L;
    case GLFW_KEY_M             : return Key::M;
    case GLFW_KEY_N             : return Key::N;
    case GLFW_KEY_O             : return Key::O;
    case GLFW_KEY_P             : return Key::P;
    case GLFW_KEY_Q             : return Key::Q;
    case GLFW_KEY_R             : return Key::R;
    case GLFW_KEY_S             : return Key::S;
    case GLFW_KEY_T             : return Key::T;
    case GLFW_KEY_U             : return Key::U;
    case GLFW_KEY_V             : return Key::V;
    case GLFW_KEY_W             : return Key::W;
    case GLFW_KEY_X             : return Key::X;
    case GLFW_KEY_Y             : return Key::Y;
    case GLFW_KEY_Z             : return Key::Z;
    case GLFW_KEY_LEFT_BRACKET  : return Key::LEFT_BRACKET;
    case GLFW_KEY_BACKSLASH     : return Key::BACKSLASH;
    case GLFW_KEY_RIGHT_BRACKET : return Key::RIGHT_BRACKET;
    case GLFW_KEY_GRAVE_ACCENT  : return Key::GRAVE_ACCENT;
    case GLFW_KEY_WORLD_1       : return Key::WORLD_1;
    case GLFW_KEY_WORLD_2       : return Key::WORLD_2;

    /* Function keys */
    case GLFW_KEY_ESCAPE        : return Key::ESCAPE;
    case GLFW_KEY_ENTER         : return Key::ENTER;
    case GLFW_KEY_TAB           : return Key::TAB;
    case GLFW_KEY_BACKSPACE     : return Key::BACKSPACE;
    case GLFW_KEY_INSERT        : return Key::INSERT;
    case GLFW_KEY_DELETE        : return Key::DELETE;
    case GLFW_KEY_RIGHT         : return Key::RIGHT;
    case GLFW_KEY_LEFT          : return Key::LEFT;
    case GLFW_KEY_DOWN          : return Key::DOWN;
    case GLFW_KEY_UP            : return Key::UP;
    case GLFW_KEY_PAGE_UP       : return Key::PAGE_UP;
    case GLFW_KEY_PAGE_DOWN     : return Key::PAGE_DOWN;
    case GLFW_KEY_HOME          : return Key::HOME;
    case GLFW_KEY_END           : return Key::END;
    case GLFW_KEY_CAPS_LOCK     : return Key::CAPS_LOCK;
    case GLFW_KEY_SCROLL_LOCK   : return Key::SCROLL_LOCK;
    case GLFW_KEY_NUM_LOCK      : return Key::NUM_LOCK;
    case GLFW_KEY_PRINT_SCREEN  : return Key::PRINT_SCREEN;
    case GLFW_KEY_PAUSE         : return Key::PAUSE;
    case GLFW_KEY_F1            : return Key::F1;
    case GLFW_KEY_F2            : return Key::F2;
    case GLFW_KEY_F3            : return Key::F3;
    case GLFW_KEY_F4            : return Key::F4;
    case GLFW_KEY_F5            : return Key::F5;
    case GLFW_KEY_F6            : return Key::F6;
    case GLFW_KEY_F7            : return Key::F7;
    case GLFW_KEY_F8            : return Key::F8;
    case GLFW_KEY_F9            : return Key::F9;
    case GLFW_KEY_F10           : return Key::F10;
    case GLFW_KEY_F11           : return Key::F11;
    case GLFW_KEY_F12           : return Key::F12;
    case GLFW_KEY_F13           : return Key::F13;
    case GLFW_KEY_F14           : return Key::F14;
    case GLFW_KEY_F15           : return Key::F15;
    case GLFW_KEY_F16           : return Key::F16;
    case GLFW_KEY_F17           : return Key::F17;
    case GLFW_KEY_F18           : return Key::F18;
    case GLFW_KEY_F19           : return Key::F19;
    case GLFW_KEY_F20           : return Key::F20;
    case GLFW_KEY_F21           : return Key::F21;
    case GLFW_KEY_F22           : return Key::F22;
    case GLFW_KEY_F23           : return Key::F23;
    case GLFW_KEY_F24           : return Key::F24;
    case GLFW_KEY_F25           : return Key::F25;
    case GLFW_KEY_KP_0          : return Key::KP_0;
    case GLFW_KEY_KP_1          : return Key::KP_1;
    case GLFW_KEY_KP_2          : return Key::KP_2;
    case GLFW_KEY_KP_3          : return Key::KP_3;
    case GLFW_KEY_KP_4          : return Key::KP_4;
    case GLFW_KEY_KP_5          : return Key::KP_5;
    case GLFW_KEY_KP_6          : return Key::KP_6;
    case GLFW_KEY_KP_7          : return Key::KP_7;
    case GLFW_KEY_KP_8          : return Key::KP_8;
    case GLFW_KEY_KP_9          : return Key::KP_9;
    case GLFW_KEY_KP_DECIMAL    : return Key::KP_DECIMAL;
    case GLFW_KEY_KP_DIVIDE     : return Key::KP_DIVIDE;
    case GLFW_KEY_KP_MULTIPLY   : return Key::KP_MULTIPLY;
    case GLFW_KEY_KP_SUBTRACT   : return Key::KP_SUBTRACT;
    case GLFW_KEY_KP_ADD        : return Key::KP_ADD;
    case GLFW_KEY_KP_ENTER      : return Key::KP_ENTER;
    case GLFW_KEY_KP_EQUAL      : return Key::KP_EQUAL;
    case GLFW_KEY_LEFT_SHIFT    : return Key::LEFT_SHIFT;
    case GLFW_KEY_LEFT_CONTROL  : return Key::LEFT_CONTROL;
    case GLFW_KEY_LEFT_ALT      : return Key::LEFT_ALT;
    case GLFW_KEY_LEFT_SUPER    : return Key::LEFT_SUPER;
    case GLFW_KEY_RIGHT_SHIFT   : return Key::RIGHT_SHIFT;
    case GLFW_KEY_RIGHT_CONTROL : return Key::RIGHT_CONTROL;
    case GLFW_KEY_RIGHT_ALT     : return Key::RIGHT_ALT;
    case GLFW_KEY_RIGHT_SUPER   : return Key::RIGHT_SUPER;
    case GLFW_KEY_MENU          : return Key::MENU;
    default: return Key::UNKNOWN;
    }
}
} // namespace

} // namespace core
