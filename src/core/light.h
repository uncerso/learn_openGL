#pragma once

#include <glm/vec3.hpp>

namespace core {

struct LightComponents {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SimpleLight {
    LightComponents components;
    glm::vec3 position;
};

struct DirectedLight {
    LightComponents components;
    glm::vec3 direction;
};

struct SpotLight {
    LightComponents components;
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct RoughProjectorLight {
    LightComponents components;
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
};


struct ProjectorLight {
    LightComponents components;
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

} // namespace core
