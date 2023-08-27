#pragma once

#include "keys.h"
#include <cstddef>

namespace std {
template < typename > struct allocator;
template < typename, typename > struct vector ;
} // namespace std

namespace core {

class Renderer {
public:
    using Renderers = std::vector<Renderer*, std::allocator<Renderer*>>;

    Renderer();

    static Renderers const & renderers();

    virtual ~Renderer() = default;
    virtual const char * name() const noexcept = 0;
    virtual void prepare() = 0;
    virtual void prepareFrameRendering();
    virtual void render() = 0;
    virtual void keyAction(KeyAction, Key) {}

    float WidthHeightRatio() const noexcept { return float(double(width) / double(height)); }
    size_t width = 1;
    size_t height = 1;
};

} // namespace core
