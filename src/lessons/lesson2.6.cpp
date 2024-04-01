#include "core/camera.h"
#include "core/light.h"
#include "core/program.h"
#include "helpers/preset.h"
#include "helpers/cube_lamp.h"
#include <array>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <string>

namespace {

#define STR(str) #str
#define STRING(str) STR(str)
#define POINT_LIGHTS 4

template<typename T>
using LightArray = std::array<T, POINT_LIGHTS>;

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

    struct DirLight {
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    struct SpotLight {
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

    struct PointLight {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };

    const int pointLightCount = )~" STRING(POINT_LIGHTS) R"~(;
    uniform vec3 uViewPos;
    uniform Material uMaterial;
    uniform DirLight uDirLight;
    uniform SpotLight uSpotLight;
    uniform PointLight uPointLight[pointLightCount];

    vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir) {
        vec3 lightDir = normalize(-uDirLight.direction);
        float diff = max(0.0, dot(lightDir, normal));

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
        vec3 ambient = uDirLight.ambient * diffuseFrag;
        vec3 diffuse = uDirLight.diffuse * diffuseFrag * diff;
        vec3 specular = uDirLight.specular * specularFrag * spec;
        return diffuse + ambient + specular;
    }

    vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir) {
        float distance = length(pointLight.position - fragPos);
        float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);

        vec3 lightDir = normalize(pointLight.position - fragPos);
        float diff = max(0.0, dot(lightDir, normal));

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
        vec3 ambient = pointLight.ambient * diffuseFrag;
        vec3 diffuse = pointLight.diffuse * diffuseFrag * diff;
        vec3 specular = pointLight.specular * specularFrag * spec;
        return attenuation * (diffuse + ambient + specular);
    }

    vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir) {
        vec3 lightDir = normalize(spotLight.position - fragPos);
        float theta = dot(lightDir, normalize(-spotLight.direction));
        float intensity = (theta - spotLight.outerCutOff) / (spotLight.cutOff - spotLight.outerCutOff);
        intensity = clamp(intensity, 0.0, 1.0);
        vec3 diffuseFrag = vec3(texture2D(uMaterial.diffuse, fTexCoords));
        vec3 ambient = spotLight.ambient * diffuseFrag;
        vec3 color = ambient;

        if (intensity > 0.0) {
            float distance = length(spotLight.position - fragPos);
            float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance);

            float diff = max(0.0, dot(lightDir, normal));

            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(0.0, dot(viewDir, reflectDir)), uMaterial.shininess);

            vec3 specularFrag = vec3(texture2D(uMaterial.specular, fTexCoords));
            vec3 diffuse = spotLight.diffuse * diffuseFrag * diff;
            vec3 specular = spotLight.specular * specularFrag * spec;
            color += intensity * attenuation * (diffuse + specular);
        }

        return color;
    }

    void main() {
        vec3 normal = normalize(fNormal);
        vec3 viewDir = normalize(uViewPos - fPos);

        vec3 color = vec3(0.0);
        color += calcDirLight(uDirLight, normal, viewDir);
        color += calcSpotLight(uSpotLight, normal, fPos, viewDir);
        for (int i = 0; i < pointLightCount; ++i)
            color += calcPointLight(uPointLight[i], normal, fPos, viewDir);

        gl_FragColor = vec4(color, 1.0);

    }
    )~";

struct Program : public core::Program {
    Program()
        : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, attributes())
        , material(*this, 0, 1)
        , dirLight(*this)
        , spotLight(*this)
        , view_pos(uniformLocation("uViewPos"))
        , model(uniformLocation("uModel"))
        , view_projection(uniformLocation("uViewProjection"))
        , normal_matrix(uniformLocation("uNormalMatrix"))
    {
        pointLights.reserve(POINT_LIGHTS);
        for (size_t i = 0; i < POINT_LIGHTS; ++i)
            pointLights.emplace_back(*this, ("uPointLight[" + std::to_string(i) + "]").c_str());
    }

    core::UniformMaterial material;
    core::UniformDirLight dirLight;
    core::UniformSpotLight spotLight;
    std::vector<core::UniformPointLight> pointLights;
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

struct Config {
    LightArray<core::PointLight> pointLights;
    core::DirLight dirLight;
    core::SpotLight spotLight;
    glm::vec3 clearColor;
};

struct RendererBase : public core::Renderer {
    RendererBase(Config&& config)
        : config(config)
    {}

    bool captureCamera() const noexcept override { return true; }

    void prepare() override {
        using namespace std::chrono_literals;
        program.emplace();
        constexpr auto& cube = prim::TEXTURED_CUBE_WITH_NORMALS;
        vbo.emplace(cube.data(), cube.size(), sizeof(cube[0]), core::BufferUsage::StaticDraw);
        drawer.emplace(*program, *vbo);
        actor.emplace(core::Camera(WidthHeightRatio(), glm::vec3{1, 1, 4}));

        material.emplace(
            core::loadResource(core::ImgResources::Container2),
            core::loadResource(core::ImgResources::Container2_specular),
            64.0f
        );

        drawer->program().material.set(*material);

        drawer->program().dirLight.set(config.dirLight);

        lamps.clear();
        lamps.reserve(config.pointLights.size());
        for (size_t i = 0; i < config.pointLights.size(); ++i) {
            lamps.emplace_back(config.pointLights[i]);
            drawer->program().pointLights[i].set(lamps.back().light);
        }
    }

    void render(float frame_delta_time) override {
        actor->precessMovement(frame_delta_time);
        auto viewProj = actor->viewProj();
        drawer->program().view_projection.set(viewProj);
        drawer->program().view_pos.set(actor->pos());

        config.spotLight.position = actor->pos(),
        config.spotLight.direction = actor->dir(),
        drawer->program().spotLight.set(config.spotLight);

        for (auto const & model_matrix : prim::TEN_CUBES_MODEL_MATRICES) {
            drawer->program().model.set(model_matrix);
            drawer->program().normal_matrix.set(core::normalMatrix(model_matrix));
            drawer->draw(core::PrimitiveType::Triangles);
        }

        for (auto & lamp : lamps)
            lamp.draw(viewProj);
    }

    void prepareFrameRendering() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(config.clearColor.r, config.clearColor.g, config.clearColor.b, 1.0f);
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
    std::vector<lamp::CubeLamp> lamps;
    std::optional<core::Material> material;

    Config config;
};


constexpr LightArray<glm::vec3> lightPos = {{
	{ 0.7f,  0.2f,  2.0f},
	{ 2.3f, -3.3f, -4.0f},
	{-4.0f,  2.0f, -12.0f},
	{ 0.0f,  0.0f, -3.0f},
}};

core::PointLight makeDefaultPointLightAt(glm::vec3 const& pos) {
    return {
        .components = {
            .ambient = glm::vec3(0.05f),
            .diffuse = glm::vec3(0.8f),
            .specular = glm::vec3(1.0f),
        },
        .position = pos,
        .constant = 0.1f,
        .linear = 0.09f,
        .quadratic = 0.032f,
    };
}

struct Task01 : RendererBase {
    Task01()
        : RendererBase({
            .pointLights = {
                makeDefaultPointLightAt(lightPos[0]),
                makeDefaultPointLightAt(lightPos[1]),
                makeDefaultPointLightAt(lightPos[2]),
                makeDefaultPointLightAt(lightPos[3]),
            },
            .dirLight = {
                .components = {
                    .ambient = glm::vec3(0.05f),
                    .diffuse = glm::vec3(0.4f),
                    .specular = glm::vec3(0.5f),
                },
                .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
            },
            .spotLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = glm::vec3(1.0f),
                    .specular = glm::vec3(1.0),
                },
                .constant = 1.0f,
                .linear = 0.09f,
                .quadratic = 0.032f,
                .cutOff = glm::cos(glm::radians(12.5f)),
                .outerCutOff = glm::cos(glm::radians(15.0f)),
            },
            .clearColor = {0.1, 0.1, 0.1},
        })
    {}
    const char * name() const noexcept override { return "2.6:0.1"; }
} instanceTask1;

struct LightAttenuationCoeff {
    float constant;
    float linear;
    float quadratic;
};

constexpr core::PointLight makePointLight(glm::vec3 const & pos, glm::vec3 const & color, LightAttenuationCoeff const & coeff) {
    return {
        .components = {
            .ambient = 0.1f * color,
            .diffuse = color,
            .specular = color,
        },
        .position = pos,
        .constant = coeff.constant,
        .linear = coeff.linear,
        .quadratic = coeff.quadratic,
    };
}

constexpr LightArray<core::PointLight> makePointLightArray(
    LightArray<glm::vec3> const & colors,
    LightArray<LightAttenuationCoeff> const & coeffs)
{
    LightArray<core::PointLight> res;
    for (size_t i = 0; i < lightPos.size(); ++i)
        res[i] = makePointLight(lightPos[i], colors[i], coeffs[i]);
    return res;
}

template <typename T>
constexpr LightArray<T> from(T const & v) {
    return {v, v, v, v};
}

struct Desert : RendererBase {
    Desert()
        : RendererBase({
            // .pointLights = makePointLightArray(pointLightColors, pointLightAttenuationCoeff),
            .pointLights = makePointLightArray(
                {{{1.0f, 0.6f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0, 0.0}, {0.2f, 0.2f, 1.0f}}},
                from<LightAttenuationCoeff>({1.0f, 0.09f, 0.032f})
            ),
            .dirLight = {
                .components = {
                    .ambient = {0.3f, 0.24f, 0.14f},
                    .diffuse = {0.7f, 0.42f, 0.26f},
                    .specular = {0.5f, 0.5f, 0.5f},
                },
                .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
            },
            .spotLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = {0.8f, 0.8f, 0.0f},
                    .specular = {0.8f, 0.8f, 0.0f},
                },
                .constant = 1.0f,
                .linear = 0.09f,
                .quadratic = 0.032f,
                .cutOff = glm::cos(glm::radians(12.5f)),
                .outerCutOff = glm::cos(glm::radians(13.0f)),
            },
            .clearColor = {0.75f, 0.52f, 0.3f},
        })
    {}
    const char * name() const noexcept override { return "2.6:desert"; }
} instanceDesert;

struct Factory : RendererBase {
    Factory()
        : RendererBase({
            .pointLights = makePointLightArray(
                {{{0.2f, 0.2f, 0.6f}, {0.3f, 0.3f, 0.7f}, {0.0f, 0.0f, 0.3f}, {0.4f, 0.4f, 0.4f}}},
                from<LightAttenuationCoeff>({1.0f, 0.09f, 0.032f})
            ),
            .dirLight = {
                .components = {
                    .ambient = {0.05f, 0.05f, 0.1f},
                    .diffuse = {0.2f, 0.2f, 0.7f},
                    .specular = {0.7f, 0.7f, 0.7f},
                },
                .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
            },
            .spotLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = glm::vec3(1.0f),
                    .specular = glm::vec3(1.0f),
                },
                .constant = 1.0f,
                .linear = 0.009f,
                .quadratic = 0.0032f,
                .cutOff = glm::cos(glm::radians(10.0f)),
                .outerCutOff = glm::cos(glm::radians(12.5f)),
            },
            .clearColor = {0.1f, 0.1f, 0.1f},
        })
    {}
    const char * name() const noexcept override { return "2.6:factory"; }
} instanceFactory;

struct Horror : RendererBase {
    Horror()
        : RendererBase({
            .pointLights = makePointLightArray(
                {{{0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f}, {0.3f, 0.1f, 0.1f}}},
                {{{1.0f, 0.14f, 0.07f}, {1.0f, 0.14f, 0.07f}, {1.0f, 0.22f, 0.2f}, {1.0f, 0.14f, 0.07f}}}
            ),
            .dirLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = glm::vec3(0.05f),
                    .specular = glm::vec3(0.2f),
                },
                .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
            },
            .spotLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = glm::vec3(1.0f),
                    .specular = glm::vec3(1.0f),
                },
                .constant = 1.0f,
                .linear = 0.09f,
                .quadratic = 0.032f,
                .cutOff = glm::cos(glm::radians(10.0f)),
                .outerCutOff = glm::cos(glm::radians(15.0f)),
            },
            .clearColor = {0.0f, 0.0f, 0.0f},
        })
    {}
    const char * name() const noexcept override { return "2.6:horror"; }
} instanceHorror;

struct BiochemicalLab : RendererBase {
    BiochemicalLab()
        : RendererBase({
            .pointLights = makePointLightArray(
                from<glm::vec3>({0.4f, 0.7f, 0.1f}),
                from<LightAttenuationCoeff>({1.0f, 0.07f, 0.017f})
            ),
            .dirLight = {
                .components = {
                    .ambient = glm::vec3(0.5f),
                    .diffuse = glm::vec3(1.0f),
                    .specular = glm::vec3(1.0f),
                },
                .direction = glm::vec3(-0.2f, -1.0f, -0.3f),
            },
            .spotLight = {
                .components = {
                    .ambient = glm::vec3(0.0f),
                    .diffuse = {0, 1, 0},
                    .specular = {0, 1, 0},
                },
                .constant = 1.0f,
                .linear = 0.07f,
                .quadratic = 0.017f,
                .cutOff = glm::cos(glm::radians(7.0f)),
                .outerCutOff = glm::cos(glm::radians(10.0f)),
            },
            .clearColor = {0.9f, 0.9f, 0.9f},
        })
    {}
    const char * name() const noexcept override { return "2.6:biochemical_lab"; }
} instanceBiochemicalLab;

} // namespace
