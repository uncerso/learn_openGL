#include "../core/renderer.h"
#include "../core/drawer.h"
#include "../core/indexer.h"
#include "../core/opengl.h"
#include "../core/animation.h"

#include <string>
#include <optional>
#include <array>
#include <numbers>
#include <cmath>
#include <iostream>

namespace {

namespace task01 {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec3 vPosition;

    void main() {
        gl_Position = vec4(vPosition, 1.0);
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    uniform vec3 uColor;
    void main() {
        gl_FragColor = vec4(uColor, 1.0);
    };
    )~";

using Triangle = std::array<glm::vec3, 3>;
using Rectangle = std::array<glm::vec3, 4>;

constexpr Triangle TRIANGLE = {{
    {-0.5f, -0.5f, 0.0f}, // Left  
    { 0.5f, -0.5f, 0.0f}, // Right 
    { 0.0f,  0.5f, 0.0f}, // Top   
}};

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , color(uniformLocation("uColor"))
    {}

    core::UniformVec3f color;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float}};
    }
};

struct Renderer_1_5_0_1 : public core::Renderer {
    const char * name() const noexcept override { return "1.5:0.1"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(TRIANGLE.data(), TRIANGLE.size(), sizeof(TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        animation.emplace(2s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render() override {
        float red_value = (std::sin(animation->progress()) + 1) / 2;
        drawer->program().color.set({red_value, 0, 0});
        drawer->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::LoopedAnimation> animation;

} instance_1_5_0_1;

} // namespace task01

namespace task02 {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec3 vPosition;
    attribute vec3 vColor;
    varying vec3 fColor;

    void main() {
        gl_Position = vec4(vPosition, 1.0);
        fColor = vColor;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fColor;
    void main() {
        gl_FragColor = vec4(fColor, 1.0);
    };
    )~";

struct ColoredVertex {
    glm::vec3 pos;
    glm::vec3 color;
};

using ColoredTriangle = std::array<ColoredVertex, 3>;

constexpr ColoredTriangle RGB_TRIANGLE = {{
    {.pos = {-0.5f, -0.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}}, // Left
    {.pos = { 0.5f, -0.5f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}}, // Right
    {.pos = { 0.0f,  0.5f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}}, // Top
}};

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
    {}

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vColor", 3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.5:0.2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(RGB_TRIANGLE.data(), RGB_TRIANGLE.size(), sizeof(RGB_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
    }

    void render() override {
        drawer->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;

} instance;

} // namespace task02

namespace task1 {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec3 vPosition;
    attribute vec3 vColor;
    varying vec3 fColor;

    uniform float angle;

    void main() {
        mat2 rot = mat2(vec2(cos(angle), sin(angle)), vec2(-sin(angle), cos(angle)));
        gl_Position = vec4(rot * vPosition.xy, vPosition.z, 1.0);
        fColor = vColor;
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, task02::FRAGMENT_SHADER_SOURCE, attributes())
        , angle(uniformLocation("angle"))
    {}

    core::UniformFloat angle;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vColor", 3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.5:1"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(task02::RGB_TRIANGLE.data(), task02::RGB_TRIANGLE.size(), sizeof(task02::RGB_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        animation.emplace(10s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render() override {
        drawer->program().angle.set(animation->progress());
        drawer->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::LoopedAnimation> animation;

} instance;

} // namespace task1

} // namespace
