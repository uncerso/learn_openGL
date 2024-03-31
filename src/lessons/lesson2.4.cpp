#include "helpers/preset.h"
#include "helpers/cube_lamp.h"

namespace {

namespace task02 {

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
    core::UniformSimpleLight light;
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
    const char * name() const noexcept override { return "2.4:0.2"; }
    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_CUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));
        lamp.emplace();

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        auto model_matrix = glm::one<glm::mat4>();
        drawer->program().model.set(model_matrix);
        drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
        drawer->program().material.set(*material);
        drawer->program().light.set({
            .components = {
                .ambient = glm::vec3(0.2f),
                .diffuse = glm::vec3(0.5f),
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

    std::optional<Program> program;
    std::optional<core::VertexBuffer> vbo;
    std::optional<core::Drawer<Program>> drawer;
    std::optional<lamp::CubeLamp> lamp;
    std::optional<core::FPSActor> actor;

    std::optional<core::Material> material;

} instance;
} // namespace task02


} // namespace
