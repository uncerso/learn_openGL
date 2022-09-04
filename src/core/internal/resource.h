#pragma once

namespace core::internal {

class Resource {
public:
    Resource() = default;

    Resource(Resource const & other) = delete;
    Resource & operator=(Resource const & other) = delete;

    Resource(Resource && other);
    Resource & operator=(Resource && other);

protected:
    unsigned int id = 0;
};

} // namespace core::internal
