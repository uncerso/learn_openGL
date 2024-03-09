#include "image_resource_loader.h"

#include <SOIL/SOIL.h>

#include <cassert>
#include <string_view>
#include <span>
#include <cstdint>

using BytesBufferView = std::span<const uint8_t>;
namespace core {

namespace {
Image::Format toFormat(int channels) {
    switch (channels) {
    case 3: return Image::Format::RGB;
    case 4: return Image::Format::RGBA;
    default: assert(false && "wrong channels");
    }
}

Image load(BytesBufferView buffer) {
    int w, h, channels;
    auto* data = SOIL_load_image_from_memory(buffer.data(), (int)buffer.size(), &w, &h, &channels, SOIL_LOAD_AUTO);
    assert(data && "cannot load image data");
    std::span<std::byte> dataView(reinterpret_cast<std::byte*>(data), size_t(w * h * channels));
    Image img {
        .width = static_cast<size_t>(w),
        .height = static_cast<size_t>(h),
        .format = toFormat(channels),
        .image = {dataView.begin(), dataView.end()}
    };
    SOIL_free_image_data(data);
    return img;
}
} // namespace


Image loadResource(ImgResources res) {
    switch (res) {
    case ImgResources::WoodContainer: return load(BytesBufferView({
        #include <resources/img/wood_container.h>
    }));
    case ImgResources::AwesomeFace: return load(BytesBufferView({
        #include <resources/img/awesomeface.h>
    }));
    case ImgResources::Container2: return load(BytesBufferView({
        #include <resources/img/container2.h>
    }));
    case ImgResources::Container2_specular: return load(BytesBufferView({
        #include <resources/img/container2_specular.h>
    }));
    }
}

} // namespace core
