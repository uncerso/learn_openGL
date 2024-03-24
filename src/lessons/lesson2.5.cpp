#include "core/camera.h"
#include "helpers/preset.h"
#include "helpers/cube_lamp.h"
#include <glm/trigonometric.hpp>

namespace {

namespace task01 {

constexpr auto VERTEX_SHADER_SOURCE =
    R"~(
    attribute vec3 vPosition;
    attribute vec2 vTexCoords;
    attribute vec3 vNormal;
    varying vec3 fNormal;
    varying vec3 fPos;
    varying vec2 fTexCoords;

    uniform mat4 uModel;
    uniform mat4 uViewProjection;
    uniform mat3 uNormalMatrix;

    void main() {
        vec4 worldPos = uModel * vec4(vPosition, 1.0);
        gl_Position = uViewProjection * worldPos;
        fPos = vec3(worldPos);
        fNormal = uNormalMatrix * vNormal;
        fTexCoords = vTexCoords;
    }
    )~";

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;
    varying vec2 fTexCoords;

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };

    struct DirectedLight {
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform DirectedLight uLight;

    void main() {
        vec3 normal = normalize(fNormal);

        vec3 lightDir = normalize(-uLight.direction);
        float diff = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
        vec3 ambient = uLight.ambient * diffuseFrag;
        vec3 diffuse = uLight.diffuse * diffuseFrag * diff;
        vec3 specular = uLight.specular * specularFrag * spec;
        gl_FragColor = vec4(diffuse + ambient + specular, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this, 0, 1)
        , light(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformMaterial material;
    core::UniformDirectedLight light;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition",  3, core::Attribute::Type::Float},
                {"vTexCoords", 2, core::Attribute::Type::Float},
                {"vNormal",    3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.5:0.1"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_QUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        drawer->program().material.set(*material);
        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(0.2f),
                .diffuse = glm::vec3(0.5f),
                .specular = glm::vec3(1.0),
            },
            .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
        });
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        for (auto const & model_matrix : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model_matrix);
            drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
            drawer->draw(core::PrimitiveType::Triangles);
        }
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
    std::optional<core::FPSActor> actor;

    std::optional<core::Material> material;

} instance;
} // namespace task01

namespace task02 {

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;
    varying vec2 fTexCoords;

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };

    struct SpotLight {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };

    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform SpotLight uLight;

    void main() {
        vec3 normal = normalize(fNormal);
        float distance = length(uLight.position - fPos);
        float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * distance * distance);

        vec3 lightDir = normalize(uLight.position - fPos);
        float diff = max(0.0, dot(lightDir, normal));

        vec3 viewDir = normalize(uViewPos - fPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
        vec3 ambient = uLight.ambient * diffuseFrag;
        vec3 diffuse = uLight.diffuse * diffuseFrag * diff;
        vec3 specular = uLight.specular * specularFrag * spec;
        gl_FragColor = vec4(attenuation * (diffuse + ambient + specular), 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task01::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this, 0, 1)
        , light(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformMaterial material;
    core::UniformSpotLight light;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition",  3, core::Attribute::Type::Float},
                {"vTexCoords", 2, core::Attribute::Type::Float},
                {"vNormal",    3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.5:0.2"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_QUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        drawer->program().material.set(*material);
        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(0.2f),
                .diffuse = glm::vec3(0.5f),
                .specular = glm::vec3(1.0),
            },
            .position = lamp->light_pos,
            .constant = 0.1f,
            .linear = 0.09f,
            .quadratic = 0.032f,
        });
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        for (auto const & model_matrix : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model_matrix);
            drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
            drawer->draw(core::PrimitiveType::Triangles);
        }
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

    std::optional<core::Material> material;

} instance;
} // namespace task02

namespace task03 {

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;
    varying vec2 fTexCoords;

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };

    struct ProjectorLight {
        vec3 position;
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
        float cutOff;
    };

    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform ProjectorLight uLight;

    void main() {
        vec3 lightDir = normalize(uLight.position - fPos);
        float theta = dot(lightDir, normalize(-uLight.direction));
        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 ambient = uLight.ambient * diffuseFrag;
        vec3 color = ambient;

        if (theta > uLight.cutOff) {
            vec3 normal = normalize(fNormal);
            float distance = length(uLight.position - fPos);
            float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * distance * distance);


            float diff = max(0.0, dot(lightDir, normal));

            vec3 viewDir = normalize(uViewPos - fPos);
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

            vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
            vec3 diffuse = uLight.diffuse * diffuseFrag * diff;
            vec3 specular = uLight.specular * specularFrag * spec;
            color += attenuation * (diffuse + specular);
        }

        gl_FragColor = vec4(color, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task01::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this, 0, 1)
        , light(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformMaterial material;
    core::UniformRoughProjectorLight light;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition",  3, core::Attribute::Type::Float},
                {"vTexCoords", 2, core::Attribute::Type::Float},
                {"vNormal",    3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.5:0.3"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_QUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        drawer->program().material.set(*material);
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(0.2f),
                .diffuse = glm::vec3(0.5f),
                .specular = glm::vec3(1.0),
            },
            .position = actor->pos(),
            .direction = actor->dir(),
            .constant = 0.1f,
            .linear = 0.09f,
            .quadratic = 0.032f,
            .cutOff = glm::cos(glm::radians(15.5f))
        });

        for (auto const & model_matrix : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model_matrix);
            drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
            drawer->draw(core::PrimitiveType::Triangles);
        }
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
    std::optional<core::FPSActor> actor;

    std::optional<core::Material> material;

} instance;
} // namespace task03

namespace task04 {

constexpr auto FRAGMENT_SHADER_SOURCE =
    R"~(
    varying vec3 fNormal;
    varying vec3 fPos;
    varying vec2 fTexCoords;

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };

    struct ProjectorLight {
        vec3 position;
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
        float cutOff;
        float outerCutOff;
    };

    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform ProjectorLight uLight;

    void main() {
        vec3 lightDir = normalize(uLight.position - fPos);
        float theta = dot(lightDir, normalize(-uLight.direction));
        float intensity = (theta - uLight.outerCutOff) / (uLight.cutOff - uLight.outerCutOff);
        intensity = clamp(intensity, 0.0, 1.0);
        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 ambient = uLight.ambient * diffuseFrag;
        vec3 color = ambient;

        if (intensity > 0.0) {
            vec3 normal = normalize(fNormal);
            float distance = length(uLight.position - fPos);
            float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * distance * distance);


            float diff = max(0.0, dot(lightDir, normal));

            vec3 viewDir = normalize(uViewPos - fPos);
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

            vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
            vec3 diffuse = uLight.diffuse * diffuseFrag * diff;
            vec3 specular = uLight.specular * specularFrag * spec;
            color += intensity * attenuation * (diffuse + specular);
        }

        gl_FragColor = vec4(color, 1.0);
    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(task01::VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this, 0, 1)
        , light(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {}

    core::UniformMaterial material;
    core::UniformProjectorLight light;
    core::UniformVec3f view_pos;
    core::UniformMat4f model;
    core::UniformMat4f view_projection;
    core::UniformMat3f normal_matrix;

    std::vector<core::Attribute> attributes() {
        return {{"vPosition",  3, core::Attribute::Type::Float},
                {"vTexCoords", 2, core::Attribute::Type::Float},
                {"vNormal",    3, core::Attribute::Type::Float}};
    }
};

struct : public core::Renderer {
    const char * name() const noexcept override { return "2.5:0.4"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_QUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        drawer->program().material.set(*material);
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(0.2f),
                .diffuse = glm::vec3(0.5f),
                .specular = glm::vec3(1.0),
            },
            .position = actor->pos(),
            .direction = actor->dir(),
            .constant = 0.1f,
            .linear = 0.09f,
            .quadratic = 0.032f,
            .cutOff = glm::cos(glm::radians(15.5f)),
            .outerCutOff = glm::cos(glm::radians(25.5f)),
        });

        for (auto const & model_matrix : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model_matrix);
            drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
            drawer->draw(core::PrimitiveType::Triangles);
        }
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
    std::optional<core::FPSActor> actor;

    std::optional<core::Material> material;

} instance;
} // namespace task04

} // namespace
