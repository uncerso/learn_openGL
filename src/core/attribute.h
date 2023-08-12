#pragma once
#include <cstdint>

namespace core {

struct Attribute {
    enum class Type {
        Float
    };

    char const * name;
    unsigned int size;
    Type type;
    bool normalize = false;
};

struct LocatedAttribute {
    LocatedAttribute(int location, unsigned int offset, Attribute const & attr)
        : location(location)
        , size(attr.size)
        , type(attr.type)
        , offset(offset)
        , normalize(attr.normalize)
    {}

    int location;
    unsigned int size;
    Attribute::Type type;
    uintptr_t offset;
    bool normalize;
};

} // namespace core
