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

namespace internal {

template <typename Result, typename ... Types, size_t N>
constexpr auto zip(std::array<Types, N> ... args) -> std::array<Result, N> {
    std::array<Result, N> res;
    for (size_t i = 0; i < N; ++i)
        res[i] = {args[i]...};
    return res;
}

} // namespace internal

constexpr size_t QUBE_POINTS = 6*6;

using Qube = std::array<glm::vec3, QUBE_POINTS>;

constexpr Qube QUBE = {{
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},

    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},

    {-0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},

    { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},

    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f},

    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f}
}};

using QubeTexCoords = std::array<glm::vec2, QUBE_POINTS>;

constexpr QubeTexCoords QUBE_TEX_COORDS = {{
    {0, 0},
    {1, 0},
    {1, 1},
    {1, 1},
    {0, 1},
    {0, 0},

    {0, 0},
    {1, 0},
    {1, 1},
    {1, 1},
    {0, 1},
    {0, 0},

    {1, 0},
    {1, 1},
    {0, 1},
    {0, 1},
    {0, 0},
    {1, 0},

    {1, 0},
    {1, 1},
    {0, 1},
    {0, 1},
    {0, 0},
    {1, 0},

    {0, 1},
    {1, 1},
    {1, 0},
    {1, 0},
    {0, 0},
    {0, 1},

    {0, 1},
    {1, 1},
    {1, 0},
    {1, 0},
    {0, 0},
    {0, 1}
}};


using QubeNormals = std::array<glm::vec3, QUBE_POINTS>;

#define REPEAT_VEC3_SIX_TIMES(a, b, c) a,b,c, a,b,c, a,b,c, a,b,c, a,b,c, a,b,c,

constexpr QubeNormals QUBE_NORMALS = {{
    REPEAT_VEC3_SIX_TIMES({ 0,  0, -1})
    REPEAT_VEC3_SIX_TIMES({ 0,  0,  1})
    REPEAT_VEC3_SIX_TIMES({-1,  0,  0})
    REPEAT_VEC3_SIX_TIMES({ 1,  0,  0})
    REPEAT_VEC3_SIX_TIMES({ 0, -1,  0})
    REPEAT_VEC3_SIX_TIMES({ 0,  1,  0})
}};

#undef REPEAT_VEC3_SIX_TIMES

struct TexturedQubeVertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
};

struct QubeVertexWithNormal {
    glm::vec3 pos;
    glm::vec3 normal;
};

struct TexturedQubeVertexWithNormal {
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

constexpr auto TEXTURED_QUBE = internal::zip<TexturedQubeVertex>(QUBE, QUBE_TEX_COORDS);
constexpr auto QUBE_WITH_NORMALS = internal::zip<QubeVertexWithNormal>(QUBE, QUBE_NORMALS);
constexpr auto TEXTURED_QUBE_WITH_NORMALS = internal::zip<TexturedQubeVertexWithNormal>(QUBE, QUBE_TEX_COORDS, QUBE_NORMALS);

} // namespace prim
