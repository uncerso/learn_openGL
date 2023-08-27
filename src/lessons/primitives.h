#pragma once

#include <glm/glm.hpp>
#include <array>

namespace prim {

struct TexturedFlatVertex {
    glm::vec2 pos;
    glm::vec2 texCoord;
};

using TexturedRectangle = std::array<TexturedFlatVertex, 4>;

constexpr TexturedRectangle TEXTURED_RECTANGLE = {{
    {{ 0.5f,  0.5f}, {1, 1-1}}, // Right Top 
    {{ 0.5f, -0.5f}, {1, 1-0}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 1-1}}, // Left Top
    {{-0.5f, -0.5f}, {0, 1-0}}, // Left Bottom
}};

struct TexturedVertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
};

struct TexturedColoredFlatVertex {
    glm::vec2 pos;
    glm::vec2 texCoord;
    glm::vec3 color;
};

using TexturedColoredRectangle = std::array<TexturedColoredFlatVertex, 4>;

constexpr TexturedColoredRectangle TEXTURED_COLORED_RECTANGLE = {{
    {{ 0.5f,  0.5f}, {1, 1-1}, {1, 0, 0}}, // Right Top 
    {{ 0.5f, -0.5f}, {1, 1-0}, {0, 1, 0}}, // Right Bottom
    {{-0.5f,  0.5f}, {0, 1-1}, {0, 0, 1}}, // Left Top
    {{-0.5f, -0.5f}, {0, 1-0}, {1, 1, 0}}, // Left Bottom
}};

using TexturedQube = std::array<TexturedVertex, 6*6>;

constexpr TexturedQube TEXTURED_QUBE = {{
    {{-0.5f, -0.5f, -0.5f},  {0, 0}},
    {{ 0.5f, -0.5f, -0.5f},  {1, 0}},
    {{ 0.5f,  0.5f, -0.5f},  {1, 1}},
    {{ 0.5f,  0.5f, -0.5f},  {1, 1}},
    {{-0.5f,  0.5f, -0.5f},  {0, 1}},
    {{-0.5f, -0.5f, -0.5f},  {0, 0}},

    {{-0.5f, -0.5f,  0.5f},  {0, 0}},
    {{ 0.5f, -0.5f,  0.5f},  {1, 0}},
    {{ 0.5f,  0.5f,  0.5f},  {1, 1}},
    {{ 0.5f,  0.5f,  0.5f},  {1, 1}},
    {{-0.5f,  0.5f,  0.5f},  {0, 1}},
    {{-0.5f, -0.5f,  0.5f},  {0, 0}},

    {{-0.5f,  0.5f,  0.5f},  {1, 0}},
    {{-0.5f,  0.5f, -0.5f},  {1, 1}},
    {{-0.5f, -0.5f, -0.5f},  {0, 1}},
    {{-0.5f, -0.5f, -0.5f},  {0, 1}},
    {{-0.5f, -0.5f,  0.5f},  {0, 0}},
    {{-0.5f,  0.5f,  0.5f},  {1, 0}},

    {{ 0.5f,  0.5f,  0.5f},  {1, 0}},
    {{ 0.5f,  0.5f, -0.5f},  {1, 1}},
    {{ 0.5f, -0.5f, -0.5f},  {0, 1}},
    {{ 0.5f, -0.5f, -0.5f},  {0, 1}},
    {{ 0.5f, -0.5f,  0.5f},  {0, 0}},
    {{ 0.5f,  0.5f,  0.5f},  {1, 0}},

    {{-0.5f, -0.5f, -0.5f},  {0, 1}},
    {{ 0.5f, -0.5f, -0.5f},  {1, 1}},
    {{ 0.5f, -0.5f,  0.5f},  {1, 0}},
    {{ 0.5f, -0.5f,  0.5f},  {1, 0}},
    {{-0.5f, -0.5f,  0.5f},  {0, 0}},
    {{-0.5f, -0.5f, -0.5f},  {0, 1}},

    {{-0.5f,  0.5f, -0.5f},  {0, 1}},
    {{ 0.5f,  0.5f, -0.5f},  {1, 1}},
    {{ 0.5f,  0.5f,  0.5f},  {1, 0}},
    {{ 0.5f,  0.5f,  0.5f},  {1, 0}},
    {{-0.5f,  0.5f,  0.5f},  {0, 0}},
    {{-0.5f,  0.5f, -0.5f},  {0, 1}}
}};

} // namespace prim
