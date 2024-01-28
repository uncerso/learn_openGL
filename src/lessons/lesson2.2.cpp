#include "helpers/preset.h"
#include "helpers/cube_lamp.h"
#include <glm/gtx/quaternion.hpp>
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

    #define AMBIENT_STRENGTH 0.1

    void main() {
        vec3 ambient = uLightColor * AMBIENT_STRENGTH;
        gl_FragColor = vec4(uObjectColor * ambient, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , object_color(uniformLocation("uObjectColor"))
        , light_color(uniformLocation("uLightColor"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
    {}

    core::UniformVec3f object_color;
    core::UniformVec3f light_color;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.2:0.2"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::QUBE.data(), prim::QUBE.size(), sizeof(prim::QUBE[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        drawer->program().model.set(glm::one<glm::mat4>());
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
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

namespace task02 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;
    attribute vec3 vNormal;
    varying vec3 fNormal;
    varying vec3 fPos;

    uniform mat4 uModel;
    uniform mat4 uViewProjection;
    uniform mat3 uNormalMatrix;

    void main() {
        vec4 worldPos = uModel * vec4(vPosition, 1.0);
        gl_Position = uViewProjection * worldPos;
        fPos = vec3(worldPos);
        fNormal = uNormalMatrix * vNormal;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;

    uniform vec3 uObjectColor;
    uniform vec3 uLightColor;
    uniform vec3 uLightPos;

    #define AMBIENT 0.1

    void main() {
        vec3 normal = normalize(fNormal);
        vec3 lightDir = normalize(uLightPos - fPos);
        float diffuse = max(0.0, dot(lightDir, normal));
        gl_FragColor = vec4(uObjectColor * uLightColor * (diffuse + AMBIENT), 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , object_color(uniformLocation("uObjectColor"))
        , light_color(uniformLocation("uLightColor"))
        , light_pos(uniformLocation("uLightPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformVec3f object_color;
    core::UniformVec3f light_color;
    core::UniformVec3f light_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vNormal",   3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.2:0.2"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::QUBE_WITH_NORMALS.data(), prim::QUBE_WITH_NORMALS.size(), sizeof(prim::QUBE_WITH_NORMALS[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        auto model_matrix = glm::one<glm::mat4>();
        drawer->program().model.set(model_matrix);
        drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
        drawer->program().light_pos.set(lamp->light_pos);
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
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
    std::optional<core::FPSActor> actor;

} instance;

} // namespace task02

namespace task03 {

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;

    uniform vec3 uObjectColor;
    uniform vec3 uLightColor;
    uniform vec3 uLightPos;
    uniform vec3 uViewPos;

    #define AMBIENT 0.1
    #define SPECULAR_STRENGTH 0.5
    #define SPECULAR_POWER 32.0

    void main() {
        vec3 normal = normalize(fNormal);

        vec3 lightDir = normalize(uLightPos - fPos);
        float diffuse = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specular = pow(max(0.0, dot(viewDir, reflectDir)), SPECULAR_POWER) * SPECULAR_STRENGTH;
        gl_FragColor = vec4(uObjectColor * uLightColor * (diffuse + AMBIENT + specular), 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task02::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , object_color(uniformLocation("uObjectColor"))
        , light_color(uniformLocation("uLightColor"))
        , light_pos(uniformLocation("uLightPos"))
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformVec3f object_color;
    core::UniformVec3f light_color;
    core::UniformVec3f light_pos;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vNormal",   3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.2:0.3"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::QUBE_WITH_NORMALS.data(), prim::QUBE_WITH_NORMALS.size(), sizeof(prim::QUBE_WITH_NORMALS[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        auto model_matrix = glm::one<glm::mat4>();
        drawer->program().model.set(model_matrix);
        drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
        drawer->program().light_pos.set(lamp->light_pos);
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().object_color.set({1.0f, 0.5f, 0.31f});
        drawer->program().light_color.set({1, 1, 1});
        drawer->program().view_pos.set(actor->pos());

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
    std::optional<core::FPSActor> actor;

} instance;

} // namespace task03

namespace task1 {

constexpr auto BASE_LIGHT_POS = glm::vec3{1.2f, 1.0f, 2.0f};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.2:1"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::QUBE_WITH_NORMALS.data(), prim::QUBE_WITH_NORMALS.size(), sizeof(prim::QUBE_WITH_NORMALS[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace(BASE_LIGHT_POS);
        animation.emplace(10s, [](float t) { return 2 * t * std::numbers::pi_v<float>; });

        auto model_matrix = glm::one<glm::mat4>();
        drawer->program().model.set(model_matrix);
        drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().object_color.set({1.0f, 0.5f, 0.31f});
        drawer->program().light_color.set({1, 1, 1});
        drawer->program().view_pos.set(actor->pos());

        lamp->light_pos = glm::rotate(glm::angleAxis(animation->progress(), glm::vec3{0, 1, 0}), BASE_LIGHT_POS);
        drawer->program().light_pos.set(lamp->light_pos);

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

    std::optional<task03::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task03::Program>> drawer;
    std::optional<lamp::CubeLamp> lamp;
    std::optional<core::FPSActor> actor;
    std::optional<core::LoopedAnimation> animation;
} instance;

} // namespace task1

namespace task4 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;
    attribute vec3 vNormal;
    varying float fLighting;

    uniform mat4 uModel;
    uniform mat4 uViewProjection;
    uniform mat3 uNormalMatrix;

    uniform vec3 uLightPos;
    uniform vec3 uViewPos;

    #define AMBIENT 0.1
    #define SPECULAR_STRENGTH 0.5
    #define SPECULAR_POWER 32.0

    void main() {
        vec4 worldPos = uModel * vec4(vPosition, 1.0);
        gl_Position = uViewProjection * worldPos;

        // Gouraud shading

        vec3 fPos = vec3(worldPos);
        vec3 fNormal = uNormalMatrix * vNormal;

        vec3 normal = normalize(fNormal);

        vec3 lightDir = normalize(uLightPos - fPos);
        float diffuse = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specular = pow(max(0.0, dot(viewDir, reflectDir)), SPECULAR_POWER) * SPECULAR_STRENGTH;

        fLighting = (diffuse + AMBIENT + specular);
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying float fLighting;

    uniform vec3 uObjectColor;
    uniform vec3 uLightColor;

    void main() {
        gl_FragColor = vec4(uObjectColor * uLightColor * fLighting, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , object_color(uniformLocation("uObjectColor"))
        , light_color(uniformLocation("uLightColor"))
        , light_pos(uniformLocation("uLightPos"))
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformVec3f object_color;
    core::UniformVec3f light_color;
    core::UniformVec3f light_pos;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition", 3, core::Attribute::Type::Float},
                {"vNormal",   3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.2:4"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        vbo.emplace(prim::QUBE_WITH_NORMALS.data(), prim::QUBE_WITH_NORMALS.size(), sizeof(prim::QUBE_WITH_NORMALS[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        auto model_matrix = glm::one<glm::mat4>();
        drawer->program().model.set(model_matrix);
        drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
        drawer->program().light_pos.set(lamp->light_pos);
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().object_color.set({1.0f, 0.5f, 0.31f});
        drawer->program().light_color.set({1, 1, 1});
        drawer->program().view_pos.set(actor->pos());

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
    std::optional<core::FPSActor> actor;

} instance;

} // namespace task4

} // namespace
