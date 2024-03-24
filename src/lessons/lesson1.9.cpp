#include "helpers/preset.h"

namespace {

namespace task01 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;
    attribute vec2 vTexCoord;
    varying vec2 fTexCoord;

    uniform mat4 uModel;
    uniform mat4 uViewProjection;

    void main() {
        gl_Position = uViewProjection * uModel * vec4(vPosition, 1.0);
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
        , model(uniformLocation("uModel"))
        , viewProjection(uniformLocation("uViewProjection"))
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;
    core::UniformFloat mixStrength;
    core::UniformMat4f model;
    core::UniformMat4f viewProjection;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.9:0.1"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_QUBE.data(), prim::TEXTURED_QUBE.size(), sizeof(prim::TEXTURED_QUBE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{0, 0, 3}));
        animation.emplace(3s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().viewProjection.set(actor->viewProj());

        float progress = animation->progress();
        for (size_t i = 0; i < prim::TEN_CUBE_POSITIONS.size(); ++i) {
            auto model = glm::translate(glm::one<glm::mat4>(), prim::TEN_CUBE_POSITIONS[i]);
            float angle = (i%3 ? glm::radians(20.0f) * float(i) : progress);
            model = glm::rotate(model, angle, {1.0f, 0.3f, 0.5f});

            drawer->program().model.set(model);
            drawer->draw(core::PrimitiveType::Triangles);
        }

        core::Texture2D::unbind();
    }

    void prepareFrameRendering() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void keyAction(core::KeyAction action, core::Key key) override {
        if (actor)
            actor->keyAction(action, key);

        if (action != core::KeyAction::Press && action != core::KeyAction::Repeat)
            return;
        if (key == core::Key::UP) {
            mixStrength += 0.1f;
        } else if (key == core::Key::DOWN) {
            mixStrength -= 0.1f;
        }
    }

    void mouseMoveDelta(glm::vec2 delta) override {
        if (actor)
            actor->mouseMoveDelta(delta);
    }

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<core::Texture2D> texture1;
    std::optional<core::Texture2D> texture2;
    std::optional<core::LoopedAnimation> animation;
    std::optional<core::Actor> actor;

    float mixStrength = 0.2f;
} instance;

} // namespace task01

} // namespace
