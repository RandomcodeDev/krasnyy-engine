// Compiler-specific defines.
//
// In general, anything non-standard should be behind a common macro, so that if in the future some compiler doesn't have
// it, it can be replaced more easily.

#pragma once

#if defined _MSC_VER
#ifdef CH_X86
#include <intrin.h>
#ifdef __clang__
#include <avxintrin.h>
#endif
#elif defined CH_XBOX360
#include <VectorIntrinsics.h>
#endif

#define ATTRIBUTE(x)                  __declspec(x)
#define EXPORT_AS_RAW(a, b)           __pragma(comment(linker, "/export:" b "=" a))
#define EXPORT_CURRENT_FUNCTION_AS(x) EXPORT_AS_RAW(__FUNCTION__, x)
#define EXPORT_AS(a, b)               EXPORT_AS_RAW(#a, #b)
#define ALIAS_RAW(a, b)               __pragma(comment(linker, "/alternatename:" b "=" a))
#define ALIAS(a, b)                   ALIAS_RAW(#a, #b)
#define BREAKPOINT()                  __debugbreak()
#define ASSUME(x)                     __assume(x)
#else
#define ATTRIBUTE(x) __attribute__((x))
#define ALIAS(a, b)  extern "C" TYPEOF(a) ATTRIBUTE(alias(#a)) b;
#define BREAKPOINT() __builtin_trap()
#define ASSUME(x)    __builtin_assume(x)
#endif

// MSVC and Clang/GCC understand all of these equally
#define TYPEOF(x) __typeof__(x)
#define NORETURN  ATTRIBUTE(noreturn)
#if _MSC_VER <= 1600
#define RESTRICT
#else
#define RESTRICT __restrict
#endif

#ifndef va_copy
#define va_copy(x, y) x = y
#endif
