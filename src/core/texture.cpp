#include "texture.h"
#include "opengl.h"

namespace core {

namespace {
GLenum toGL(Image::Format format) {
    switch (format) {
    case Image::Format::RGB: return GL_RGB;
    case Image::Format::RGBA: return GL_RGBA;
    }
}

GLint toGLInternalFormat(Image::Format format) {
    switch (format) {
    case Image::Format::RGB: return GL_RGB;
    case Image::Format::RGBA: return GL_RGBA;
    }
}

GLint toGl(Texture2D::Wrap::Type type) {
    switch (type) {
    case Texture2D::Wrap::Type::Repeat:         return GL_REPEAT;
    case Texture2D::Wrap::Type::MirroredRepeat: return GL_MIRRORED_REPEAT;
    case Texture2D::Wrap::Type::ClampToEdge:    return GL_CLAMP_TO_EDGE;
    case Texture2D::Wrap::Type::ClampToBorder:  return GL_CLAMP_TO_BORDER;
    }
}

GLint toGl(Texture2D::Filter type) {
    switch (type) {
        case Texture2D::Filter::Nearest: return GL_NEAREST;
        case Texture2D::Filter::Linear: return GL_LINEAR;
    }
}

} // namespace

Texture2D::Texture2D(const Image& image, Config config) {
    glGenTextures(1, &id);

    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, toGLInternalFormat(image.format), GLsizei(image.width), GLsizei(image.height), 0, toGL(image.format), GL_UNSIGNED_BYTE, image.image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGl(config.wrap.s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGl(config.wrap.t));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGl(config.minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGl(config.magFilter));

    unbind();
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &id);
}

void Texture2D::bind() const { glBindTexture(GL_TEXTURE_2D, id); }
void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

} // namespace core
