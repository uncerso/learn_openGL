#include "helpers/preset.h"
#include "helpers/cube_lamp.h"

namespace {

namespace task01 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;

    uniform mat4 uModel;
    uniform mat4 uViewProjection;

    void main() {
        gl_Position = uViewProjection * uModel * vec4(vPosition, 1.0);
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    uniform vec3 uObjectColor;
    uniform vec3 uLightColor;

    void main() {
        gl_FragColor = vec4(uObjectColor * uLightColor, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , object_color(uniformLocation("uObjectColor"))
        , light_color(uniformLocation("uLightColor"))
        , model(uniformLocation("uModel"))
        , viewProjection(uniformLocation("uViewProjection"))
    {}

    core::UniformVec3f object_color;
    core::UniformVec3f light_color;
    core::UniformMat4f model;
    core::UniformMat4f viewProjection;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.1:0.1"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::CUBE.data(), prim::CUBE.size(), sizeof(prim::CUBE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        drawer->program().model.set(glm::one<glm::mat4>());
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().viewProjection.set(viewProj);
        drawer->program().object_color.set({1.0f, 0.5f, 0.31f});
        drawer->program().light_color.set({1, 1, 1});

        drawer->draw(core::PrimitiveType::Triangles);
        lamp->draw(viewProj);
    }

    void prepareFrameRendering() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void keyAction(core::KeyAction action, core::Key key) override {
        if (actor)
            actor->keyAction(action, key);
    }

    void mouseMoveDelta(glm::vec2 delta) override {
        if (actor)
            actor->mouseMoveDelta(delta);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<lamp::CubeLamp> lamp;
    std::optional<core::Actor> actor;

} instance;

} // namespace task01

} // namespace
