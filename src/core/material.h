#pragma once

#include <glm/vec3.hpp>
#include <core/texture.h>

namespace core {

struct SimpleMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Material {
    Texture2D diffuse;
    Texture2D specular;
    float shininess;
};

} // namespace core
