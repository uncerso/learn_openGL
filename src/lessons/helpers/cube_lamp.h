#include "../../core/vertex_buffer.h"
#include "../../core/drawer.h"
#include "../../core/light.h"
#include "core/program.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace lamp {

class CubeLamp {
    struct Program : core::Program {
        Program();
        core::UniformMat4f model;
        core::UniformMat4f viewProjection;
        core::UniformVec3f color;
    };

public:
    CubeLamp(glm::vec3 light_pos = glm::vec3{1.2f, 1.0f, 2.0f});
    CubeLamp(core::PointLight light);

    void draw(glm::mat4 const & viewProj);

    core::SimpleLight simpleLight() const;

    core::PointLight light;
private:
    Program program;
    core::VertexBuffer vbo;
    core::Drawer<Program> drawer;
};

} // namespace lamp
