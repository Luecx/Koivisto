#pragma once

#include <iostream>

// Adapted from https://stackoverflow.com/questions/53986620/how-to-implement-a-standard-compliant-assert-macro-with-an-optional-formatted-me

#define nitpick_assert(expression, msg)                                         \
do {                                                                            \
    if(!(expression))                                                           \
    {                                                                           \
        std::cerr << __FILE__ << ":" << __LINE__ << " nitpick_assert: " << msg; \
        abort();                                                                \
    }                                                                           \
} while(0)