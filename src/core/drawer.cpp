#include "drawer.h"
#include "opengl.h"
#include "exception.h"

#include <cassert>
#include <string>

namespace core {

namespace {

GLenum toGL(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Points        : return GL_POINTS;
        case PrimitiveType::Lines         : return GL_LINES;
        case PrimitiveType::LineStrip     : return GL_LINE_STRIP;
        case PrimitiveType::LineLoop      : return GL_LINE_LOOP;
        case PrimitiveType::Triangles     : return GL_TRIANGLES;
        case PrimitiveType::TriangleStrip : return GL_TRIANGLE_STRIP;
        case PrimitiveType::TriangleFan   : return GL_TRIANGLE_FAN;
        default: assert(false && "unreachable");
    }
}

void checkElementsCount(PrimitiveType type, size_t count) {
    switch (type) {
        case PrimitiveType::Points:
            break;
        case PrimitiveType::Lines:
            REQUIRE(count % 2 == 0, "Cannot draw lines: the number of points should be even");
            break;
        case PrimitiveType::LineStrip:
            REQUIRE(count >= 2, "Cannot draw a line strip: the number of points should be at least two");
            break;
        case PrimitiveType::LineLoop:
            REQUIRE(count >= 2, "Cannot draw a line loop: the number of points should be at least two");
            break;
        case PrimitiveType::Triangles:
            REQUIRE(count % 3 == 0, "Cannot draw triangles: the number of points should be a multiple of three");
            break;
        case PrimitiveType::TriangleStrip:
            REQUIRE(count >= 3, "Cannot draw a triangle strip: the number of points should be at least three");
            break;
        case PrimitiveType::TriangleFan:
            REQUIRE(count >= 3, "Cannot draw a triangle fan: the number of points should be at least three");
            break;
        default: assert(false && "unreachable");
    }
}

} // namespace


DrawerBase::DrawerBase(Program & program, VertexBuffer const & vbo, IndexBuffer const * ibo)
    : active_program(program)
    , vbo(vbo)
    , ibo(ibo)
{
    glGenVertexArrays(1, &id);

    bind();
    if (ibo)
        ibo->bind();
    vbo.bind();
    program.enableAttributes();
    unbind();
}

DrawerBase::~DrawerBase() {
    // A value of 0 will be silently ignored.
    glDeleteVertexArrays(1, &id);
}

void DrawerBase::bind() const {
    glBindVertexArray(id);
}

void DrawerBase::unbind() {
    glBindVertexArray(0);
}

void DrawerBase::draw(PrimitiveType type, size_t from, size_t size) {
    checkElementsCount(type, size);
    assert(from + size <= (ibo ? ibo->size() : vbo.size()));

    bind();
    active_program.use();

    if (ibo) {
        static_assert(std::is_same_v<IndexBuffer::IndexType, unsigned int>);
        glDrawElements(toGL(type), static_cast<GLsizei>(size), GL_UNSIGNED_INT, reinterpret_cast<void*>(from * sizeof(unsigned int)));
    } else {
        glDrawArrays(toGL(type), static_cast<GLint>(from), static_cast<GLsizei>(size));
    }

    active_program.disuse();
    unbind();
}

void DrawerBase::draw(PrimitiveType type) {
    if (ibo) {
        draw(type, 0, ibo->size());
    } else {
        draw(type, 0, vbo.size());
    }
}

} // namespace core
