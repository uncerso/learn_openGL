#pragma once

#include "internal/buffer.h"

namespace core {

class IndexBuffer : public internal::Buffer<internal::BufferType::Index> {
public:
    DEFAULT_MOVABLE(IndexBuffer);

    using IndexType = unsigned int;

    IndexBuffer(IndexType const * data, size_t number_of_elements, BufferUsage usage)
        : Buffer(reinterpret_cast<std::byte const *>(data), number_of_elements, sizeof(IndexType), usage)
    {}
};

} // namespace core
