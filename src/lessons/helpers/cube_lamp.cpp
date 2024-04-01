#include "cube_lamp.h"
#include "preset.h"

namespace lamp {

namespace {
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
    uniform vec3 uColor;
    void main() {
        gl_FragColor = vec4(uColor, 1.0);
    }
    )~";

std::vector<core::Attribute> attributes() {
    return {{"vPosition", 3, core::Attribute::Type::Float}};
}
} // namespace

CubeLamp::Program::Program()
    : core::Program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, lamp::attributes())
    , model(uniformLocation("uModel"))
    , viewProjection(uniformLocation("uViewProjection"))
    , color(uniformLocation("uColor"))
{}

CubeLamp::CubeLamp(glm::vec3 light_pos)
    : CubeLamp({
        .components = {
            .ambient = glm::vec3(0.2f),
            .diffuse = glm::vec3(0.5f),
            .specular = glm::vec3(1.0),
        },
        .position = light_pos,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f,
    })
{}

CubeLamp::CubeLamp(core::PointLight light)
    : light(std::move(light))
    , vbo(prim::CUBE.data(), prim::CUBE.size(), sizeof(prim::CUBE[0]), core::BufferUsage::StaticDraw)
    , drawer(program, vbo)
{}

void CubeLamp::draw(glm::mat4 const & viewProj) {
    glm::mat4 model = glm::translate(glm::one<glm::mat4>(), light.position);
    model = glm::scale(model, glm::vec3(0.2f));
    drawer.program().model.set(model);
    drawer.program().color.set(light.components.diffuse);

    drawer.program().viewProjection.set(viewProj);
    drawer.draw(core::PrimitiveType::Triangles);
}

core::SimpleLight CubeLamp::simpleLight() const {
    return {
        .components = light.components,
        .position = light.position,
    };
}

} // namespace lamp
