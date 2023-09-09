#include "../../core/vertex_buffer.h"
#include "../../core/drawer.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace lamp {

class CubeLamp {
    struct Program : core::Program {
        Program();
        core::UniformMat4f model;
        core::UniformMat4f viewProjection;
    };
    
public:
    CubeLamp(glm::vec3 light_pos = glm::vec3{1.2f, 1.0f, 2.0f});

    void draw(glm::mat4 const & viewProj);

    glm::vec3 const light_pos;
private:
    Program program;
    core::VertexBuffer vbo;
    core::Drawer<Program> drawer;
};

} // namespace lamp
