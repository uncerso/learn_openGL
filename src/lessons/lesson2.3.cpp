#include "helpers/preset.h"
#include "helpers/cube_lamp.h"

namespace {

namespace task01 {

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

    uniform vec3 uLightColor;
    uniform vec3 uLightPos;
    uniform vec3 uViewPos;

    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    uniform Material uMaterial;

    #define AMBIENT 0.1

    void main() {
        vec3 normal = normalize(fNormal);

        vec3 lightDir = normalize(uLightPos - fPos);
        float diffuse = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specular = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);
        gl_FragColor = vec4(uLightColor * (diffuse * uMaterial.diffuse + AMBIENT * uMaterial.ambient + specular * uMaterial.specular), 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this)
        , light_color(uniformLocation("uLightColor"))
        , light_pos(uniformLocation("uLightPos"))
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformSimpleMaterial material;
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
    const char * name() const noexcept override { return "2.3:0.1"; }
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

        drawer->program().light_color.set({1, 1, 1});
        drawer->program().material.set({
            .ambient = {1.0f, 0.5f, 0.31f},
            .diffuse = {1.0f, 0.5f, 0.31f},
            .specular = {0.5f, 0.5f, 0.5f},
            .shininess = 32.0f,
        });
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
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

} // namespace task01

namespace task02 {

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;

    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    struct Light {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform Light uLight;

    void main() {
        vec3 normal = normalize(fNormal);

        vec3 lightDir = normalize(uLight.position - fPos);
        float diff = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

        vec3 ambient = uLight.ambient * uMaterial.ambient;
        vec3 diffuse = uLight.diffuse * uMaterial.diffuse * diff;
        vec3 specular = uLight.specular * uMaterial.specular * spec;
        gl_FragColor = vec4(diffuse + ambient + specular, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task01::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this)
        , light(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformSimpleMaterial material;
    core::UniformSimpleLight light;
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
    const char * name() const noexcept override { return "2.3:0.2"; }
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

        drawer->program().material.set({
            .ambient = {1.0f, 0.5f, 0.31f},
            .diffuse = {1.0f, 0.5f, 0.31f},
            .specular = {0.5f, 0.5f, 0.5f},
            .shininess = 32.0f,
        });

        drawer->program().light.set(lamp->light());
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
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
} // namespace task02

namespace task03 {
struct : public core::Renderer {
    const char * name() const noexcept override { return "2.3:0.3"; }
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

        drawer->program().material.set({
            .ambient = {1.0f, 0.5f, 0.31f},
            .diffuse = {1.0f, 0.5f, 0.31f},
            .specular = {0.5f, 0.5f, 0.5f},
            .shininess = 32.0f,
        });

        xColor.emplace(10s, [](float t) { return sin(2 * t * std::numbers::pi_v<float> * 2.0f) / 2.0f + 0.5f; });
        yColor.emplace(10s, [](float t) { return sin(2 * t * std::numbers::pi_v<float> * 0.7f) / 2.0f + 0.5f; });
        zColor.emplace(10s, [](float t) { return sin(2 * t * std::numbers::pi_v<float> * 1.3f) / 2.0f + 0.5f; });
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        core::SimpleLight light = lamp->light();
        light.components.diffuse = 0.5f * glm::vec3(xColor->progress(), yColor->progress(), zColor->progress());
        light.components.ambient = 0.2f * light.components.diffuse;
        drawer->program().light.set(light);

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

    std::optional<task02::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task02::Program>> drawer;
    std::optional<lamp::CubeLamp> lamp;
    std::optional<core::FPSActor> actor;
    std::optional<core::LoopedAnimation> xColor;
    std::optional<core::LoopedAnimation> yColor;
    std::optional<core::LoopedAnimation> zColor;

} instance;
} // namespace task03

namespace task1 {
struct : public core::Renderer {
    const char * name() const noexcept override { return "2.3:1"; }
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

        drawer->program().material.set({
            .ambient = {0.0f, 0.1f, 0.06f},
            .diffuse = {0.0f, 0.50980392f, 0.50980392f},
            .specular = {0.50196078f, 0.50196078f, 0.50196078f},
            .shininess = 32.0f,
        });

        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(1.0),
                .diffuse = glm::vec3(1.0),
                .specular = glm::vec3(1.0),
            },
            .position = lamp->light_pos,
        });
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
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

    std::optional<task02::Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<task02::Program>> drawer;
    std::optional<lamp::CubeLamp> lamp;
    std::optional<core::FPSActor> actor;
} instance;
} // namespace task1


} // namespace
