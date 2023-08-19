#pragma once

#include "image.h"
#include "internal/resource.h"

namespace core {

class Texture2D : internal::Resource {
public:
    enum class Filter { Nearest, Linear };

    struct Wrap {
        enum class Type {
            Repeat,
            MirroredRepeat,
            ClampToEdge,
            ClampToBorder
        };

        Wrap() noexcept {}
        Wrap(Type type) : s(type), t(type) {}

        Type s = Type::Repeat;
        Type t = Type::Repeat;
    };

    struct Config {
        Config() noexcept {}
        Wrap wrap = { Wrap::Type::Repeat };
        Filter minFilter = Filter::Linear;
        Filter magFilter = Filter::Linear;
        // TODO: vec4 borderColor;
    };

    Texture2D(const Image& image, Config config = {});
    ~Texture2D();

    void bind() const;
    static void unbind();
};

} // namespace core
