#pragma once

#include "internal/resource.h"
#include "internal/movable.h"

#include "program.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include <functional>

namespace core {

enum class PrimitiveType {
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Triangles,
    TriangleStrip,
    TriangleFan
};

class DrawerBase : internal::Resource {
protected:
    DrawerBase(Program & program, VertexBuffer const & vbo, IndexBuffer const * ibo = nullptr);
    ~DrawerBase();

public:
    DrawerBase(DrawerBase&&) = default;

    void draw(PrimitiveType type, size_t from, size_t size);
    void draw(PrimitiveType type);

private:
    void bind() const;
    static void unbind();

protected:
    Program & active_program;
    VertexBuffer const & vbo;
    IndexBuffer const * ibo;
};

template<class ProgramType>
class Drawer : public DrawerBase {
public:
    Drawer(ProgramType & program, VertexBuffer const & vbo)
        : DrawerBase(program, vbo)
    {}

    Drawer(ProgramType & program, VertexBuffer const & vbo, IndexBuffer const & ibo)
        : DrawerBase(program, vbo, &ibo)
    {}

    ProgramType & program() {
        active_program.use();
        return *reinterpret_cast<ProgramType*>(&active_program);
    }
};

} // namespace core
