#pragma once

#include <glm/vec3.hpp>

namespace core {

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

} // namespace core
