#pragma once

#include <cstddef>
#include <vector>

namespace core {

struct Image {
    enum Format { RGB, RGBA };

    size_t width;
    size_t height;
    Format format;
    std::vector<std::byte> image;
};

} // namespace core
