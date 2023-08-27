#include "renderer.h"
#include "opengl.h"

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

void Renderer::prepareFrameRendering() {
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace core
