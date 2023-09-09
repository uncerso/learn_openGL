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
    void main() {
        gl_FragColor = vec4(1.0);
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
{}

CubeLamp::CubeLamp(glm::vec3 light_pos)
    : light_pos(light_pos)
    , vbo(prim::QUBE.data(), prim::QUBE.size(), sizeof(prim::QUBE[0]), core::BufferUsage::StaticDraw)
    , drawer(program, vbo)
{}

void CubeLamp::draw(glm::mat4 const & viewProj) {
    glm::mat4 model = glm::translate(glm::one<glm::mat4>(), light_pos);
    model = glm::scale(model, glm::vec3(0.2f));
    drawer.program().model.set(model);
 
    drawer.program().viewProjection.set(viewProj);
    drawer.draw(core::PrimitiveType::Triangles);
}

} // namespace lamp
