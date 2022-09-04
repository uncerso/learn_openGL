#include "program.h"
#include "opengl.h"
#include "exception.h"

#include <string>
#include <cassert>

namespace core {

namespace {

constexpr int INVALID_LOCATION = -1;

unsigned int sizeOf(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::Float: return sizeof(float);
        default: assert(false && "unreachable");
    }
}

unsigned int toGL(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::Float: return GL_FLOAT;
        default: assert(false && "unreachable");
    }
}

struct Shader {
    enum class Type { vertex, fragment };

    GLuint shader;

    Shader(Type type, const char * shader_source) 
        : shader(glCreateShader(toGLenum(type)))
    {
        glShaderSource(shader, 1, &shader_source, NULL);
        glCompileShader(shader);

        checkShaderError(type);
    }

    ~Shader() {
        glDeleteShader(shader);
    }

private:
    void checkShaderError(Type type) const {
        constexpr size_t size = 1024;
        GLchar infoLog[size];
        GLint success;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, size, NULL, infoLog);
            REQUIRE(false, toErrorMsg(type) + ":\n" + infoLog);
        }
    }

    GLenum toGLenum(Type type) const {
        switch (type) {
            case Type::vertex: return GL_VERTEX_SHADER;
            case Type::fragment: return GL_FRAGMENT_SHADER;
            default: assert(false && "unreachable");
        }
    }

    std::string toErrorMsg(Type type) const {
        using namespace std::string_literals;
        switch (type) {
            case Type::vertex: return "Cannot compile vertex shader"s;
            case Type::fragment: return "Cannot compile fragment shader"s;
            default: assert(false && "unreachable");
        }
    }
};

void checkProgramError(GLuint program) {
    using namespace std::string_literals;
    constexpr size_t size = 1024;
    GLchar infoLog[size];
    GLint success;

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, size, NULL, infoLog);
        REQUIRE(false, "Cannot link shader program:\n"s + infoLog);
    }
}

} // namespace

Program::Program(
    const char * vertex_shader_source,
    const char * fragment_shader_source,
    std::vector<Attribute> const & attrs)
{
    Shader vertex_shader(Shader::Type::vertex, vertex_shader_source);
    Shader fragment_shader(Shader::Type::fragment, fragment_shader_source);

    id = glCreateProgram();
    glAttachShader(id, vertex_shader.shader);
    glAttachShader(id, fragment_shader.shader);
    glLinkProgram(id);
    checkProgramError(id);

    loadAttributes(attrs);
}

Program::~Program() {
    // A value of 0 will be silently ignored.
    glDeleteProgram(id);
}

void Program::use() {
    glUseProgram(id);
}

void Program::disuse() {
    glUseProgram(0);
}

int Program::uniformLocation(char const * name) const {
    using namespace std::string_literals;

    int location = glGetUniformLocation(id, name);
    REQUIRE(location != INVALID_LOCATION, "There is no such uniform: "s + name);
    return location;
}

int Program::attributeLocation(char const * name) const {
    using namespace std::string_literals;

    int location = glGetAttribLocation(id, name);
    REQUIRE(location != INVALID_LOCATION, "There is no such attribute: "s + name);
    return location;
}

void Program::loadAttributes(std::vector<Attribute> const & attrs) {
    using namespace std::string_literals;

    stride = 0;
    attributes.reserve(attrs.size());

    for (auto const & attr : attrs) {
        attributes.emplace_back(attributeLocation(attr.name), stride, attr);
        stride += sizeOf(attr.type) * attr.size;
    }
}

UniformBase::UniformBase(UniformBase && other)
    : location(other.location)
{
    other.location = 0;
}

UniformBase & UniformBase::operator=(UniformBase && other) {
    std::swap(location, other.location);
    return *this;
}

void UniformFloat::set(float value) {
    glUniform1f(location, value);
}

void Program::enableAttributes() const {
    for (auto const & attr : attributes) {
        glVertexAttribPointer(attr.location, attr.size, toGL(attr.type), attr.offset, stride, (const void *)attr.normalize);
        glEnableVertexAttribArray(attr.location);
    }
}

void Program::disableAttributes() const {
    for (auto const & attr : attributes)
        glDisableVertexAttribArray(attr.location);
}

} // namespace core
