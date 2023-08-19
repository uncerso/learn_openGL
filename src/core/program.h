#pragma once

#include "internal/resource.h"
#include "internal/movable.h"
#include "attribute.h"
#include "texture.h"

#include <vector>

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
    void set(float v1, float v2);
};

struct UniformVec3f : UniformBase {
    UniformVec3f(int location) : UniformBase(location) {}
    void set(float v1, float v2, float v3);
};

struct UniformVec4f : UniformBase {
    UniformVec4f(int location) : UniformBase(location) {}
    void set(float v1, float v2, float v3, float v4);
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

} // namespace core
