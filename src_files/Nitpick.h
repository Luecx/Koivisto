#pragma once

#include <iostream>

// Adapted from https://stackoverflow.com/questions/53986620/how-to-implement-a-standard-compliant-assert-macro-with-an-optional-formatted-me

#define nitpick_assert(expression, msg)                                                      \
do {                                                                                         \
    if(!(expression))                                                                        \
    {                                                                                        \
        std::cerr << __FILE__ << "@" << __LINE__ << " nitpick_assert: " << msg << std::endl; \
        abort();                                                                             \
    }                                                                                        \
} while(0);


// Simple tracing macros to indent logging messages.
// Use nitpick_simple_trace_enter when entering a scope, which will increase indentation.
// Use nitpick_simple_trace_exit when exiting.
// You can use nitpick_simple_trace_log to log without changing indentation.

// TODO: check for negative indentation (can happen if mismatching enter/exit)

extern int nitpick_simple_trace_indentation;

// Enter a scope, increase indentation and log.
#define nitpick_simple_trace_enter(msg)                                                           \
do {                                                                                              \
    for(int i = 0; i < nitpick_simple_trace_indentation; i++) { std::cerr << "  "; }              \
    std::cerr << __FILE__ << "@" << __LINE__ << "(";                                              \
    std::cerr << nitpick_simple_trace_indentation << ") trace:  ";                                \
    std::cerr << msg << std::endl;                                                                \
    nitpick_simple_trace_indentation++;                                                           \
} while(0)


// Log at the current indentation level
#define nitpick_simple_trace_log(msg)                                                             \
do {                                                                                              \
    for(int i = 0; i < nitpick_simple_trace_indentation; i++) { std::cerr << "  "; }              \
    std::cerr << __FILE__ << "@" << __LINE__ << "(";                                              \
    std::cerr << nitpick_simple_trace_indentation << ") trace:  ";                                \
    std::cerr << msg << std::endl;                                                                \
} while(0)

// Exit a scope and decrease indentation.
#define nitpick_simple_trace_exit(msg)                                                            \
do {                                                                                              \
    nitpick_simple_trace_indentation--;                                                           \
    for(int i = 0; i < nitpick_simple_trace_indentation; i++) { std::cerr << "  "; }              \
    std::cerr << __FILE__ << "@" << __LINE__ << "(";                                              \
    std::cerr << nitpick_simple_trace_indentation << ") trace:  ";                                \
    std::cerr << msg << std::endl;                                                                \
} while(0)

