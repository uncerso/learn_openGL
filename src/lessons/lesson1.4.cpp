#include "../core/renderer.h"
#include "../core/drawer.h"
#include "../core/indexer.h"
#include "../core/opengl.h"

#include <string>
#include <optional>
#include <array>

namespace {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec3 vPosition;

    void main() {
        gl_Position = vec4(vPosition, 1.0);
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    void main() {
        gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    };
    )~";

constexpr auto YELLOW_FRAGMENT_SHADER_SOURCE =
    R"~(
    void main() {
        gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    };
    )~";

using Triangle = std::array<glm::vec3, 3>;
using Rectangle = std::array<glm::vec3, 4>;

constexpr Triangle TRIANGLE = {{
        {-0.5f, -0.5f, 0.0f}, // Left  
        { 0.5f, -0.5f, 0.0f}, // Right 
        { 0.0f,  0.5f, 0.0f}, // Top   
    }};

constexpr Triangle RIGHT_TOP_TRIANGLE = {{
        { 0.5f,  0.5f, 0.0f}, // Right Top 
        { 0.5f, -0.5f, 0.0f}, // Right Bottom
        {-0.5f,  0.5f, 0.0f}, // Left Top
    }};

constexpr Triangle LEFT_BOTTOM_TRIANGLE = {{
        { 0.5f, -0.5f, 0.0f}, // Right Bottom
        {-0.5f, -0.5f, 0.0f}, // Left Bottom
        {-0.5f,  0.5f, 0.0f}, // Left Top
    }};

constexpr Rectangle RECTANGLE = {{
    { 0.5f,  0.5f, 0.0f}, // Right Top 
    { 0.5f, -0.5f, 0.0f}, // Right Bottom
    {-0.5f,  0.5f, 0.0f}, // Left Top
    {-0.5f, -0.5f, 0.0f}, // Left Bottom
}};

struct Program : public core::Program {
    Program(const char * fragment_shader = FRAGMENT_SHADER_SOURCE)
        : core::Program(VERTEX_SHADER_SOURCE, fragment_shader, attributes())
    {}

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.4:0.1"; }

    void prepare() override {
        program.emplace();
        vbo.emplace(TRIANGLE.data(), TRIANGLE.size(), sizeof(TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
    }

    void render() override {
        drawer->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;

} instance_0_1;

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.4:0.2"; }

    void prepare() override {
        core::Indexer indices;
        indices.addQuad();

        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        ibo.emplace(indices.data(), indices.size(), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo, *ibo);
    }

    void render() override {
        drawer->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::IndexBuffer> ibo;
    std::optional<core::Drawer<Program>> drawer;

} instance_0_2;

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.4:1"; }

    void prepare() override {
        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
    }

    void render() override {
        drawer->draw(core::PrimitiveType::TriangleStrip);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;

} instance_1;

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.4:2"; }

    void prepare() override {
        program.emplace();
        vbo1.emplace(RIGHT_TOP_TRIANGLE.data(), RIGHT_TOP_TRIANGLE.size(), sizeof(RIGHT_TOP_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer1.emplace(*program, *vbo1);
        vbo2.emplace(LEFT_BOTTOM_TRIANGLE.data(), LEFT_BOTTOM_TRIANGLE.size(), sizeof(LEFT_BOTTOM_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer2.emplace(*program, *vbo2);
    }

    void render() override {
        drawer1->draw(core::PrimitiveType::Triangles);
        drawer2->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo1;
    std::optional<core::Drawer<Program>> drawer1;
    std::optional<core::VertexBuffer> vbo2;
    std::optional<core::Drawer<Program>> drawer2;

} instance_2;

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.4:3"; }

    void prepare() override {
        program.emplace();
        vbo1.emplace(RIGHT_TOP_TRIANGLE.data(), RIGHT_TOP_TRIANGLE.size(), sizeof(RIGHT_TOP_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer1.emplace(*program, *vbo1);

        yellowProgram.emplace(YELLOW_FRAGMENT_SHADER_SOURCE);
        vbo2.emplace(LEFT_BOTTOM_TRIANGLE.data(), LEFT_BOTTOM_TRIANGLE.size(), sizeof(LEFT_BOTTOM_TRIANGLE[0]), core::BufferUsage::StaticDraw);
        drawer2.emplace(*yellowProgram, *vbo2);
    }

    void render() override {
        drawer1->draw(core::PrimitiveType::Triangles);
        drawer2->draw(core::PrimitiveType::Triangles);
    }

    std::optional<Program> program;
    std::optional<Program> yellowProgram;
    std::optional<core::VertexBuffer> vbo1;
    std::optional<core::Drawer<Program>> drawer1;
    std::optional<core::VertexBuffer> vbo2;
    std::optional<core::Drawer<Program>> drawer2;

} instance_3;


} // namespace
