#pragma once

#define REQUIRE(cond, msg) \
    do {                   \
        if (!(cond)) {     \
            throw (msg);   \
        }                  \
    } while (false)
