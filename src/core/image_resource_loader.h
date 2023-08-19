#pragma once

#include "image.h"

namespace core {

enum class ImgResources {
    WoodContainer,
    AwesomeFace
};

Image loadResource(ImgResources res);

} // namespace core
