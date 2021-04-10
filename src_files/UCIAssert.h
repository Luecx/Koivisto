#ifndef UCI_ASSERT_H
#define UCI_ASSERT_H

#include <iostream>

#ifdef NDEBUG
#define UCI_ASSERT(expr)
#else
#define UCI_ASSERT(expr)                                                                                               \
    {                                                                                                                  \
        if (!static_cast<bool>(expr)) {                                                                                \
            std::cout << "info string assert failed" << std::endl;                                                     \
            std::cout << "info string expression " << (#expr) << std::endl;                                            \
            std::cout << "info string file " << __FILE__ << std::endl;                                                 \
            std::cout << "info string line " << __LINE__ << std::endl;                                                 \
            std::cout << "info string function " << __PRETTY_FUNCTION__ << std::endl;                                  \
            std::exit(1);                                                                                              \
        }                                                                                                              \
    }
#endif

#endif
