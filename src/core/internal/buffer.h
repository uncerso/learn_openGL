#pragma once

#include "resource.h"
#include "movable.h"
#include <cstddef>

namespace core {

enum class BufferUsage {
    StaticDraw,
    DynamicDraw,
    StreamDraw
};

namespace internal {

enum class BufferType { Vertex, Index };

template<BufferType type>
class Buffer : Resource {
public:
    DEFAULT_MOVABLE(Buffer);

    Buffer(const std::byte* data, size_t number_of_elements, size_t element_size, BufferUsage usage);

    ~Buffer();

    void bind() const;
    static void unbind();

    size_t size() const noexcept { return number_of_elements; }

private:
    void load(const std::byte* data, size_t total_size, BufferUsage usage);
    size_t number_of_elements;
};

} // namespace internal

} // namespace core
