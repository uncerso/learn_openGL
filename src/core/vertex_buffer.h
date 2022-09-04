#pragma once

#include "internal/buffer.h"

namespace core {

class VertexBuffer : public internal::Buffer<internal::BufferType::Vertex> {
public:
    DEFAULT_MOVABLE(VertexBuffer);

    template<class T>
    VertexBuffer(T const * data, size_t number_of_elements, size_t element_size, BufferUsage usage)
        : Buffer(reinterpret_cast<std::byte const *>(data), number_of_elements, element_size, usage)
    {}
};

} // namespace core
