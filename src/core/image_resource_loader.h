#pragma once

#include "image.h"

namespace core {

enum class ImgResources {
    WoodContainer,
    AwesomeFace,
    Container2,
    Container2_specular,
};

Image loadResource(ImgResources res);

} // namespace core
