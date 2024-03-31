#pragma once

#include "internal/resource.h"
#include "internal/movable.h"
#include "attribute.h"
#include "light.h"
#include "material.h"
#include "texture.h"

#include <string_view>
#include <vector>
#include <glm/glm.hpp>

namespace core {

class Program : internal::Resource {
public:
    DEFAULT_MOVABLE(Program);

    Program(
        const char * vertex_shader_source,
        const char * fragment_shader_source,
        std::vector<Attribute> const & attrs);

    ~Program();

    void use();
    static void disuse();

    int uniformLocation(char const * name) const;
    int uniformLocation(std::string const & name) const;

    void enableAttributes() const;
    void disableAttributes() const;

private:
    int attributeLocation(char const * name) const;
    void loadAttributes(std::vector<Attribute> const & attrs);

    std::vector<LocatedAttribute> attributes;
    unsigned int stride = 0;
};

class UniformBase {
public:
    UniformBase(UniformBase const & other) = delete;
    UniformBase & operator=(UniformBase const & other) = delete;

    UniformBase(UniformBase && other);
    UniformBase & operator=(UniformBase && other);

protected:
    UniformBase(int location) : location(location) {}
    int location;
};

struct UniformFloat : UniformBase {
    UniformFloat(int location) : UniformBase(location) {}
    void set(float value);
};

struct UniformVec2f : UniformBase {
    UniformVec2f(int location) : UniformBase(location) {}
    void set(glm::vec2 v);
};

struct UniformVec3f : UniformBase {
    UniformVec3f(int location) : UniformBase(location) {}
    void set(glm::vec3 v);
};

struct UniformVec4f : UniformBase {
    UniformVec4f(int location) : UniformBase(location) {}
    void set(glm::vec4 v);
};

struct UniformMat2f : UniformBase {
    UniformMat2f(int location) : UniformBase(location) {}
    void set(glm::mat2 v);
};

struct UniformMat3f : UniformBase {
    UniformMat3f(int location) : UniformBase(location) {}
    void set(glm::mat3 v);
};

struct UniformMat4f : UniformBase {
    UniformMat4f(int location) : UniformBase(location) {}
    void set(glm::mat4 v);
};

struct UniformTexture : UniformBase {
    UniformTexture(int location, int texture_block)
        : UniformBase(location)
        , texture_block(texture_block)
    {}

    void set(Texture2D const & texture);
private:
    int texture_block;
};

struct UniformSimpleMaterial {
    UniformSimpleMaterial(Program & program);
    void set(SimpleMaterial const & material);
private:
    UniformVec3f ambient;
    UniformVec3f diffuse;
    UniformVec3f specular;
    UniformFloat shininess;
};

struct UniformMaterial {
    UniformMaterial(Program & program, int diffuse_texture_block, int specular_texture_block);
    void set(Material const & material);
private:
    UniformTexture diffuse;
    UniformTexture specular;
    UniformFloat shininess;
};

struct UniformLightComponents {
    UniformLightComponents(Program & program, char const * prefix);
    void set(LightComponents const & light);
private:
    UniformVec3f ambient;
    UniformVec3f diffuse;
    UniformVec3f specular;
};

struct UniformSimpleLight {
    UniformSimpleLight(Program & program, char const * prefix = "uLight");
    void set(SimpleLight const & light);
private:
    UniformLightComponents components;
    UniformVec3f position;
};

struct UniformDirLight {
    UniformDirLight(Program & program, char const * prefix = "uDirLight");
    void set(DirLight const & light);
private:
    UniformLightComponents components;
    UniformVec3f direction;
};

struct UniformPointLight {
    UniformPointLight(Program & program, char const * prefix = "uPointLight");
    void set(PointLight const & light);
private:
    UniformLightComponents components;
    UniformVec3f position;
    UniformFloat constant;
    UniformFloat linear;
    UniformFloat quadratic;
};

struct UniformRoughSpotLight {
    UniformRoughSpotLight(Program & program, char const * prefix = "uSpotLight");
    void set(RoughSpotLight const & light);
private:
    UniformLightComponents components;
    UniformVec3f position;
    UniformVec3f direction;
    UniformFloat constant;
    UniformFloat linear;
    UniformFloat quadratic;
    UniformFloat cutOff;
};

struct UniformSpotLight {
    UniformSpotLight(Program & program, char const * prefix = "uSpotLight");
    void set(SpotLight const & light);
private:
    UniformLightComponents components;
    UniformVec3f position;
    UniformVec3f direction;
    UniformFloat constant;
    UniformFloat linear;
    UniformFloat quadratic;
    UniformFloat cutOff;
    UniformFloat outerCutOff;
};

} // namespace core
