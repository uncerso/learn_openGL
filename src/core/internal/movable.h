#pragma once

#define DEFAULT_MOVABLE(ClassName) \
    ClassName(ClassName&&) = default;   \
    ClassName & operator=(ClassName&&) = default
