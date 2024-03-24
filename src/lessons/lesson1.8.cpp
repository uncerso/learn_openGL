#include "helpers/preset.h"

namespace {

namespace task01 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec2 vPosition;
    attribute vec2 vTexCoord;
    varying vec2 fTexCoord;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    void main() {
        gl_Position = uProjection * uView * uModel * vec4(vPosition, 0.0, 1.0);
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
        , view(uniformLocation("uView"))
        , projection(uniformLocation("uProjection"))
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;
    core::UniformFloat mixStrength;
    core::UniformMat4f model;
    core::UniformMat4f view;
    core::UniformMat4f projection;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 2, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "1.8:0.1"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_RECTANGLE.data(), prim::TEXTURED_RECTANGLE.size(), sizeof(prim::TEXTURED_RECTANGLE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));

        model = glm::rotate(glm::one<glm::mat4>(), glm::radians(-55.0f), {1, 0, 0});
        view = glm::translate(glm::one<glm::mat4>(), {0, 0, -3});
        projection = glm::perspective(45.0f, WidthHeightRatio(), 0.1f, 100.0f);
    }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);

        drawer->program().model.set(model);
        drawer->program().view.set(view);
        drawer->program().projection.set(projection);
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
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

} instance;

} // namespace task01

namespace task02 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;
    attribute vec2 vTexCoord;
    varying vec2 fTexCoord;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    void main() {
        gl_Position = uProjection * uView * uModel * vec4(vPosition, 1.0);
        fTexCoord = vTexCoord;
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, task01::FRAGMENT_SHADER_SOURCE, attributes())
        , texture1(uniformLocation("sample1"), 0)
        , texture2(uniformLocation("sample2"), 1)
        , mixStrength(uniformLocation("uMixStrength"))
        , model(uniformLocation("uModel"))
        , view(uniformLocation("uView"))
        , projection(uniformLocation("uProjection"))
    {}

    core::UniformTexture texture1;
    core::UniformTexture texture2;
    core::UniformFloat mixStrength;
    core::UniformMat4f model;
    core::UniformMat4f view;
    core::UniformMat4f projection;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vTexCoord", 2, core::Attribute::Type::Float}};
    }
};

struct Task02 : public core::Renderer {
    const char * name() const noexcept override { return "1.8:0.2"; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::TEXTURED_QUBE.data(), prim::TEXTURED_QUBE.size(), sizeof(prim::TEXTURED_QUBE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        texture1.emplace(core::loadResource(core::ImgResources::WoodContainer));
        texture2.emplace(core::loadResource(core::ImgResources::AwesomeFace));
        view = glm::translate(glm::one<glm::mat4>(), {0, 0, -3});
        projection = glm::perspective(45.0f, WidthHeightRatio(), 0.1f, 100.0f);
        animation.emplace(3s, [](auto t) { return 2 * t * std::numbers::pi_v<float>; } );
    }

    void render() override {
        auto model = glm::rotate(glm::one<glm::mat4>(), animation->progress(), {0.5, 1, 0});

        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().model.set(model);
        drawer->program().view.set(view);
        drawer->program().projection.set(projection);

        drawer->draw(core::PrimitiveType::Triangles);

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
    std::optional<core::LoopedAnimation> animation;
    glm::mat4 view;
    glm::mat4 projection;

    float mixStrength = 0.2f;
} instance;

} // namespace task02

namespace task03 {

struct Task03 : task02::Task02 {
    const char * name() const noexcept override { return "1.8:0.3"; }

    void prepareFrameRendering() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
} instance;

} // namespace task03

namespace task04 {

struct Task04 : task03::Task03 {
    const char * name() const noexcept override { return "1.8:0.4"; }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().view.set(view);
        drawer->program().projection.set(projection);

        for (auto const & model : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model);
            drawer->draw(core::PrimitiveType::Triangles);
        }

        core::Texture2D::unbind();
    }

} instance;

} // namespace task04

namespace task3 {

struct : task04::Task04 {
    const char * name() const noexcept override { return "1.8:3"; }

    void render() override {
        drawer->program().texture1.set(*texture1);
        drawer->program().texture2.set(*texture2);
        drawer->program().mixStrength.set(mixStrength);
        drawer->program().view.set(view);
        drawer->program().projection.set(projection);

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

} instance;

} // namespace task3

} // namespace
