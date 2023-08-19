#include "image_resource_loader.h"

#include <SOIL/SOIL.h>

#include <cassert>
#include <string_view>
#include <span>

namespace core {
#define AS_RES(p) ("../resources/img/" p)

namespace {
Image::Format toFormat(int channels) {
    switch (channels) {
    case 3: return Image::Format::RGB;
    case 4: return Image::Format::RGBA;
    default: assert(false && "wrong channels");
    }
}

Image load(std::string_view path) {
    int w, h, channels;
    // todo: use embed and SOIL_load_image_from_memory
    auto* data = SOIL_load_image(path.data(), &w, &h, &channels, SOIL_LOAD_AUTO);
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
    case ImgResources::WoodContainer: return load(AS_RES("wood_container.jpg"));
    case ImgResources::AwesomeFace: return load(AS_RES("awesomeface.png"));
    }
}

} // namespace core
