#pragma once

#include <cstdlib>
#include <iostream>
#include <source_location>

// Debug assertion macro that only runs in debug builds
// Completely optimized out in release builds
#ifdef NDEBUG
// Release build - no runtime cost
#define LMPASSERT(condition) ((void)0)
#else
// Debug build - assertions active
#define LMPASSERT(condition)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            auto loc = std::source_location::current();                                                                \
            std::cerr << "Assertion failed: " << #condition << "\n"                                                    \
                      << "File: " << loc.file_name() << "\n"                                                           \
                      << "Line: " << loc.line() << "\n"                                                                \
                      << "Function: " << loc.function_name() << std::endl;                                             \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)
#endif

// Optional: Variant that always executes regardless of build mode
#define LMPASSERT_ALWAYS(condition)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            auto loc = std::source_location::current();                                                                \
            std::cerr << "Assertion failed: " << #condition << "\n"                                                    \
                      << "File: " << loc.file_name() << "\n"                                                           \
                      << "Line: " << loc.line() << "\n"                                                                \
                      << "Function: " << loc.function_name() << std::endl;                                             \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)