#include "resource.h"

#include <utility>

namespace core::internal {

Resource::Resource(Resource && other)
    : id(other.id)
{
    other.id = 0;
}

Resource & Resource::operator=(Resource && other) {
    std::swap(id, other.id);
    return *this;
}

} // namespace core::internal
