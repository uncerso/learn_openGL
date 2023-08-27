#include "../core/renderer.h"
#include "../core/drawer.h"
#include "../core/indexer.h"
#include "../core/opengl.h"
#include "../core/animation.h"
#include "../core/image_resource_loader.h"

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
    attribute vec2 vPosition;
    attribute vec2 vTexCoord;
    varying vec2 fTexCoord;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        fTexCoord = vTexCoord;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE = 
    R"~(
    uniform sampler2D sample;
    varying vec2 fTexCoord;

    void main() {
        gl_FragColor = texture2D(sample, fTexCoord);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , texture(uniformLocation("sample"), 0)
    {}

    core::UniformTexture texture;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct TexturedVertex {
    glm::vec2 pos;
    glm::vec2 texCoord;
};

using TexturedRectangle = std::array<TexturedVertex, 4>;

constexpr TexturedRectangle RECTANGLE = {{
    {{ 0.5f,  0.5f}, {1, 1}}, // Right Top 
    {{ 0.5f, -0.5f}, {1, 0}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 1}}, // Left Top
    {{-0.5f, -0.5f}, {0, 0}}, // Left Bottom
}};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.6:0.1"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture.emplace(core::loadResource(core::ImgResources::WoodContainer));
    }

    void render() override {
        drawer->program().texture.set(*texture);
        texture->bind();
        drawer->draw(core::PrimitiveType::TriangleStrip);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::Texture2D> texture;

} instance;

} // namespace task01

namespace task02 {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec2 vPosition;
    attribute vec2 vTexCoord;
    attribute vec3 vColor;
    varying vec2 fTexCoord;
    varying vec3 fColor;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        fTexCoord = vTexCoord;
        fColor = vColor;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE = 
    R"~(
    uniform sampler2D sample;
    varying vec2 fTexCoord;
    varying vec3 fColor;

    void main() {
        gl_FragColor = texture2D(sample, fTexCoord) * vec4(fColor, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , texture(uniformLocation("sample"), 0)
    {}

    core::UniformTexture texture;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float},
                {"vColor",    3, core::Attribute::Type::Float}};
    }
};

struct TexturedColoredVertex {
    glm::vec2 pos;
    glm::vec2 texCoord;
    glm::vec3 color;
};

using TexturedColoredRectangle = std::array<TexturedColoredVertex, 4>;

constexpr TexturedColoredRectangle RECTANGLE = {{
    {{ 0.5f,  0.5f}, {1, 1}, {1, 0, 0}}, // Right Top 
    {{ 0.5f, -0.5f}, {1, 0}, {0, 1, 0}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 1}, {0, 0, 1}}, // Left Top
    {{-0.5f, -0.5f}, {0, 0}, {1, 1, 0}}, // Left Bottom
}};


struct : public core::Renderer {
    const char * name() const noexcept override { return "1.6:0.2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture.emplace(core::loadResource(core::ImgResources::WoodContainer));
    }

    void render() override {
        drawer->program().texture.set(*texture);
        drawer->draw(core::PrimitiveType::TriangleStrip);
        core::Texture2D::unbind();
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::Texture2D> texture;

} instance;

} // namespace task02

namespace task03 {

constexpr auto VERTEX_SHADER_SOURCE = 
    R"~(
    attribute vec2 vPosition;
    attribute vec2 vTexCoord;
    varying vec2 fTexCoord;

    void main() {
        gl_Position = vec4(vPosition, 0.0, 1.0);
        fTexCoord = vTexCoord;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE = 
    R"~(
    uniform sampler2D sample1;
    uniform sampler2D sample2;
    varying vec2 fTexCoord;

    void main() {
        gl_FragColor = mix(texture2D(sample1, fTexCoord), texture2D(sample2, fTexCoord), 0.2);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , texture1(uniformLocation("sample1"), 0)
        , texture2(uniformLocation("sample2"), 1)
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

constexpr task01::TexturedRectangle RECTANGLE = {{
    {{ 0.5f,  0.5f}, {1, 1-1}}, // Right Top 
    {{ 0.5f, -0.5f}, {1, 1-0}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 1-1}}, // Left Top
    {{-0.5f, -0.5f}, {0, 1-0}}, // Left Bottom
}};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.6:0.3"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
    }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->draw(core::PrimitiveType::TriangleStrip);
        core::Texture2D::unbind();
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;

} instance;

} // namespace task03

namespace task2 {

constexpr task01::TexturedRectangle RECTANGLE = {{
    {{ 0.5f,  0.5f}, {2, 0}}, // Right Top 
    {{ 0.5f, -0.5f}, {2, 2}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 0}}, // Left Top
    {{-0.5f, -0.5f}, {0, 2}}, // Left Bottom
}};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.6:2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(RECTANGLE.data(), RECTANGLE.size(), sizeof(RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
    }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->draw(core::PrimitiveType::TriangleStrip);
        core::Texture2D::unbind();
    }

    std::optional<task03::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task03::Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;

} instance;

} // namespace task2

namespace task4 {

constexpr auto FRAGMENT_SHADER_SOURCE = 
    R"~(
    uniform sampler2D sample1;
    uniform sampler2D sample2;
    uniform float uMixStrength;
    varying vec2 fTexCoord;

    void main() {
        gl_FragColor = mix(texture2D(sample1, fTexCoord), texture2D(sample2, fTexCoord), uMixStrength);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task03::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , texture1(uniformLocation("sample1"), 0)
        , texture2(uniformLocation("sample2"), 1)
        , mixStrength(uniformLocation("uMixStrength"))
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;
    core::UniformFloat mixStrength;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.6:4"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(task03::RECTANGLE.data(), task03::RECTANGLE.size(), sizeof(task03::RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
    }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->draw(core::PrimitiveType::TriangleStrip);
        core::Texture2D::unbind();
    }

    void keyAction(core::KeyAction action, core::Key key) override {
        if (action != core::KeyAction::Press && action != core::KeyAction::Repeat)
            return;
        if (key == core::Key::UP) {
            mixStrength += 0.1f;
        } else if (key == core::Key::DOWN) {
            mixStrength -= 0.1f;
        }
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;
    float mixStrength = 0.2f;

} instance;

} // namespace task4
} // namespace
