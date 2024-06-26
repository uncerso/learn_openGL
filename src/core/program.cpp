#include "program.h"
#include "opengl.h"
#include "exception.h"

#include <string>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>

namespace core {

using namespace std::string_literals;

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

int Program::uniformLocation(std::string const & name) const {
    return uniformLocation(name.c_str());
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

void UniformVec2f::set(glm::vec2 v) {
    glUniform2f(location, v.x, v.y);
}

void UniformVec3f::set(glm::vec3 v) {
    glUniform3f(location, v.x, v.y, v.z);
}

void UniformVec4f::set(glm::vec4 v) {
    glUniform4f(location, v.x, v.y, v.z, v.w);
}

void UniformMat2f::set(glm::mat2 v) {
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

void UniformMat3f::set(glm::mat3 v) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

void UniformMat4f::set(glm::mat4 v) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

void UniformTexture::set(Texture2D const & texture) {
    glActiveTexture(GL_TEXTURE0 + texture_block);
    texture.bind();
    glUniform1i(location, texture_block);
}

UniformSimpleMaterial::UniformSimpleMaterial(Program & program)
    : ambient(program.uniformLocation("uMaterial.ambient"))
    , diffuse(program.uniformLocation("uMaterial.diffuse"))
    , specular(program.uniformLocation("uMaterial.specular"))
    , shininess(program.uniformLocation("uMaterial.shininess"))
{}

void UniformSimpleMaterial::set(SimpleMaterial const & material) {
    ambient.set(material.ambient);
    diffuse.set(material.diffuse);
    specular.set(material.specular);
    shininess.set(material.shininess);
}

UniformMaterial::UniformMaterial(Program & program, int diffuse_texture_block, int specular_texture_block)
    : diffuse(program.uniformLocation("uMaterial.diffuse"), diffuse_texture_block)
    , specular(program.uniformLocation("uMaterial.specular"), specular_texture_block)
    , shininess(program.uniformLocation("uMaterial.shininess"))
{}

void UniformMaterial::set(Material const & material) {
    diffuse.set(material.diffuse);
    specular.set(material.specular);
    shininess.set(material.shininess);
}

UniformLightComponents::UniformLightComponents(Program & program, char const * prefix)
    : ambient(program.uniformLocation(prefix + ".ambient"s))
    , diffuse(program.uniformLocation(prefix + ".diffuse"s))
    , specular(program.uniformLocation(prefix + ".specular"s))
{}

void UniformLightComponents::set(LightComponents const & light) {
    ambient.set(light.ambient);
    diffuse.set(light.diffuse);
    specular.set(light.specular);
}


UniformSimpleLight::UniformSimpleLight(Program & program, char const * prefix)
    : components(program, prefix)
    , position(program.uniformLocation(prefix + ".position"s))
{}

void UniformSimpleLight::set(SimpleLight const & light) {
    components.set(light.components);
    position.set(light.position);
}

UniformDirLight::UniformDirLight(Program & program, char const * prefix)
    : components(program, prefix)
    , direction(program.uniformLocation(prefix + ".direction"s))
{}

void UniformDirLight::set(DirLight const & light) {
    components.set(light.components);
    direction.set(light.direction);
}

UniformPointLight::UniformPointLight(Program & program, char const * prefix)
    : components(program, prefix)
    , position(program.uniformLocation(prefix + ".position"s))
    , constant(program.uniformLocation(prefix + ".constant"s))
    , linear(program.uniformLocation(prefix + ".linear"s))
    , quadratic(program.uniformLocation(prefix + ".quadratic"s))
{}

void UniformPointLight::set(PointLight const & light) {
    components.set(light.components);
    position.set(light.position);
    constant.set(light.constant);
    linear.set(light.linear);
    quadratic.set(light.quadratic);
}

UniformRoughSpotLight::UniformRoughSpotLight(Program & program, char const * prefix)
    : components(program, prefix)
    , position(program.uniformLocation(prefix + ".position"s))
    , direction(program.uniformLocation(prefix + ".direction"s))
    , constant(program.uniformLocation(prefix + ".constant"s))
    , linear(program.uniformLocation(prefix + ".linear"s))
    , quadratic(program.uniformLocation(prefix + ".quadratic"s))
    , cutOff(program.uniformLocation(prefix + ".cutOff"s))
{}

void UniformRoughSpotLight::set(RoughSpotLight const & light) {
    components.set(light.components);
    position.set(light.position);
    direction.set(light.direction);
    constant.set(light.constant);
    linear.set(light.linear);
    quadratic.set(light.quadratic);
    cutOff.set(light.cutOff);
}

UniformSpotLight::UniformSpotLight(Program & program, char const * prefix)
    : components(program, prefix)
    , position(program.uniformLocation(prefix + ".position"s))
    , direction(program.uniformLocation(prefix + ".direction"s))
    , constant(program.uniformLocation(prefix + ".constant"s))
    , linear(program.uniformLocation(prefix + ".linear"s))
    , quadratic(program.uniformLocation(prefix + ".quadratic"s))
    , cutOff(program.uniformLocation(prefix + ".cutOff"s))
    , outerCutOff(program.uniformLocation(prefix + ".outerCutOff"s))
{}

void UniformSpotLight::set(SpotLight const & light) {
    components.set(light.components);
    position.set(light.position);
    direction.set(light.direction);
    constant.set(light.constant);
    linear.set(light.linear);
    quadratic.set(light.quadratic);
    cutOff.set(light.cutOff);
    outerCutOff.set(light.outerCutOff);
}

void Program::enableAttributes() const {
    for (auto const & attr : attributes) {
        glVertexAttribPointer(attr.location, attr.size, toGL(attr.type), attr.normalize, stride, (const GLvoid *)attr.offset);
        glEnableVertexAttribArray(attr.location);
    }
}

void Program::disableAttributes() const {
    for (auto const & attr : attributes)
        glDisableVertexAttribArray(attr.location);
}

} // namespace core
