#pragma once

#include "keys.h"
#include <glm/vec2.hpp>
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
    virtual void render(float /*frame_delta_time*/) { render(); }
    virtual void render() {}
    virtual void keyAction(KeyAction, Key) {}
    virtual void mouseMoveDelta(glm::vec2 /*delta*/) {}
    virtual void mouseMove(glm::vec2 /*pos*/) {}
    virtual bool captureCamera() const noexcept { return false; }

    float WidthHeightRatio() const noexcept { return float(double(width) / double(height)); }
    size_t width = 1;
    size_t height = 1;
};

} // namespace core
