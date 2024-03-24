#include "buffer.h"
#include "../opengl.h"

#include <cassert>

namespace core {

GLenum toGL(BufferUsage type) {
    switch (type) {
        case BufferUsage::StaticDraw: return GL_STATIC_DRAW;
        case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
        case BufferUsage::StreamDraw: return GL_STREAM_DRAW;
        default: assert(false && "unreachable");
    }
}

namespace internal {

template<BufferType type>
consteval GLenum toGL() {
    switch (type) {
        case BufferType::Vertex: return GL_ARRAY_BUFFER;
        case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
        default: assert(false && "unreachable");
    }
}

template<BufferType type>
Buffer<type>::Buffer(const std::byte* data, size_t number_of_elements, size_t element_size, BufferUsage usage)
    : number_of_elements(number_of_elements)
{
    glGenBuffers(1, &id);
    load(data, number_of_elements * element_size, usage);
}

template<BufferType type>
Buffer<type>::~Buffer() {
    // A value of 0 will be silently ignored.
    glDeleteBuffers(1, &id);
}

template<BufferType type>
void Buffer<type>::bind() const {
    glBindBuffer(toGL<type>(), id);
}

template<BufferType type>
void Buffer<type>::unbind() {
    glBindBuffer(toGL<type>(), 0);
}

template<BufferType type>
void Buffer<type>::load(const std::byte* data, size_t size, BufferUsage usage) {
    bind();
    glBufferData(toGL<type>(), static_cast<GLsizeiptr>(size), data, toGL(usage));
}

template class Buffer<BufferType::Vertex>;
template class Buffer<BufferType::Index>;

} // namespace internal

} // namespace core
