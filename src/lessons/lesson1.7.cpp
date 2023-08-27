#include "primitives.h"

#include "../core/renderer.h"
#include "../core/drawer.h"
#include "../core/indexer.h"
#include "../core/opengl.h"
#include "../core/animation.h"
#include "../core/image_resource_loader.h"

#include <glm/gtc/matrix_transform.hpp>

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

    uniform mat4 uMVP;

    void main() {
        gl_Position = uMVP * vec4(vPosition, 0.0, 1.0);
        fTexCoord = vTexCoord;
    }
    )~";

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
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , texture1(uniformLocation("sample1"), 0)
        , texture2(uniformLocation("sample2"), 1)
        , mixStrength(uniformLocation("uMixStrength"))
        , mvp(uniformLocation("uMVP"))
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;
    core::UniformFloat mixStrength;
    core::UniformMat4f mvp;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.7:0.1"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_RECTANGLE.data(), prim::TEXTURED_RECTANGLE.size(), sizeof(prim::TEXTURED_RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));

        mvp = glm::rotate(glm::one<glm::mat4>(), glm::radians(90.0f), {0, 0, 1});
        mvp = glm::scale(mvp, {0.5f, 0.5f, 0.5f});
    }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().mvp.set(mvp);
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
    glm::mat4 mvp;

} instance;

} // namespace task01

namespace task02 {

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.7:0.2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_RECTANGLE.data(), prim::TEXTURED_RECTANGLE.size(), sizeof(prim::TEXTURED_RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
        animation.emplace(2s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render() override {
        auto mvp = glm::translate(glm::one<glm::mat4>(), {0.5, -0.5, 0});
        mvp = glm::rotate(mvp, animation->progress(), {0, 0, 1});

        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().mvp.set(mvp);
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

    std::optional<task01::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task01::Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;
    std::optional<core::LoopedAnimation> animation;
    float mixStrength = 0.2f;

} instance;

} // namespace task02

namespace task2 {

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.7:2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_RECTANGLE.data(), prim::TEXTURED_RECTANGLE.size(), sizeof(prim::TEXTURED_RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
        animation.emplace(2s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render() override {
        auto mvp1 = glm::translate(glm::one<glm::mat4>(), {0.5, -0.5, 0});
        mvp1 = glm::rotate(mvp1, animation->progress(), {0, 0, 1});
        auto mvp2 = glm::translate(glm::one<glm::mat4>(), {-0.5, 0.5, 0});
        mvp2 = glm::rotate(mvp2, animation->progress(), {0, 0, 1});

        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().mvp.set(mvp1);
        drawer->draw(core::PrimitiveType::TriangleStrip);
        drawer->program().mvp.set(mvp2);
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

    std::optional<task01::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task01::Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;
    std::optional<core::LoopedAnimation> animation;
    float mixStrength = 0.2f;

} instance;

} // namespace task2
} // namespace
