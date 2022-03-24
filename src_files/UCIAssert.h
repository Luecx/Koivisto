#ifndef UCI_ASSERT_H
#define UCI_ASSERT_H

#include <iostream>

#if defined(__GNUC__) || defined(__clang__)
#define FUNCTION_NAME __PRETTY_FUNCTION__
#elif _MSC_VER
#define FUNCTION_NAME __FUNCSIG__
#else
#define FUNCTION_NAME __func__
#endif

#ifdef NDEBUG
#define UCI_ASSERT(expr)
#else
#define UCI_ASSERT(expr)                                                                             \
    {                                                                                                \
        if (!static_cast<bool>(expr)) {                                                              \
            std::cout << "info string assert failed" << std::endl;                                   \
            std::cout << "info string expression " << (#expr) << std::endl;                          \
            std::cout << "info string file " << __FILE__ << std::endl;                               \
            std::cout << "info string line " << __LINE__ << std::endl;                               \
            std::cout << "info string function " << FUNCTION_NAME << std::endl;                      \
            std::exit(1);                                                                            \
        }                                                                                            \
    }
#endif

#endif
