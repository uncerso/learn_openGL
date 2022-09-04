#pragma once

#include <vector>
#include <cstddef>

namespace core {

struct Indexer {
    using IndexType = unsigned int;

    void addTriangle(IndexType a, IndexType b, IndexType c) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
    }

    void addQuad(IndexType a, IndexType b, IndexType c, IndexType d) {
        addTriangle(a, b, c);
        addTriangle(d, b, c);
    }

    void addQuad() {
        addQuad(0, 1, 2, 3);
    }

    IndexType * data() { return indices.data(); }
    size_t size() { return indices.size(); }

    std::vector<IndexType> indices;
};


} // namespace core
