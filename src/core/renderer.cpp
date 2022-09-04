#include "renderer.h"

#include <vector>

namespace core {

namespace {

std::vector<Renderer*> all_renderers;

} // namespace


Renderer::Renderer() {
    all_renderers.push_back(this);
}

std::vector<Renderer*> const & Renderer::renderers() {
    return all_renderers;
}

} // namespace core
