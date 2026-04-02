#pragma once
#include "Log.h"

// Use EPOCH_ASSERT when:
//	- Internal invariant
//	- Programmer error
//	- Should never happen

// Use EPOCH_VERIFY when:
//	- Expression must run
//	- You still want validation in dev builds


#if defined(_MSC_VER)
#define EPOCH_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define EPOCH_DEBUG_BREAK() __builtin_trap() /*__builtin_debugtrap()*/
#else
#define EPOCH_DEBUG_BREAK()
#endif

#if defined(_DEBUG) || defined(_RELEASE)
#define EPOCH_ENABLE_ASSERTS
#endif

#ifdef EPOCH_ENABLE_ASSERTS

#define ASSERT_MESSAGE_INTERNAL(...) ::Epoch::Log::PrintAssertMessage(__VA_ARGS__)

#define EPOCH_ASSERT(condition, ...)                                      \
    do {                                                                  \
        if (!(condition)) {                                               \
            ::Epoch::Log::PrintAssertMessage(                             \
                #condition, __FILE__, __LINE__, __FUNCTION__,             \
                ##__VA_ARGS__                                             \
            );                                                            \
            EPOCH_DEBUG_BREAK();                                          \
        }                                                                 \
    } while (0)

#else
#define EPOCH_ASSERT(condition, ...) ((void)0)
#endif

#ifdef EPOCH_ENABLE_ASSERTS
#define EPOCH_VERIFY(condition, ...) EPOCH_ASSERT(condition, __VA_ARGS__)
#else
#define EPOCH_VERIFY(condition, ...) (condition)
#endif
