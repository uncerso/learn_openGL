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

struct DirLight {
    LightComponents components;
    glm::vec3 direction;
};

struct PointLight {
    LightComponents components;
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct RoughSpotLight {
    LightComponents components;
    glm::vec3 position;
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
};


struct SpotLight {
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
