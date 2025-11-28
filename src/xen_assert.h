#pragma once
#include <cstdio>
//#include <iostream>

//#define ERROR_IDX_INBOUNDS(x, y) {if((int)x >= (int)y || (int)x < 0){ App::GetInstance()->PrintError("idx {} out of bounds, size 0 through {}", (int)x, (int)y); }}

//#define ERROR_ASSERT(x) do{if(!(x)){ std::cout << "assertion failed" << std::endl; __debugbreak(); }}while(0)
//#define ERROR_IDX_INBOUNDS(idx, size) do{if((((int)idx) >= ((int)size)) || (((int)idx) < 0)){ std::cout << "index out of bounds, size of" << std::endl; __debugbreak(); }}while(0)
//#define ERROR_NOT_NULL(x) do{if((x) == nullptr){ std::cout << "error, is null" << std::endl; __debugbreak(); }}while(0)

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() ((void)0)
#endif

// The ASSERT macro
#ifdef NDEBUG
  // In release builds, disable it (do nothing)
#define ASSERT(expr) ((void)0)
#define ASSERT_INBOUNDS(expr) ((void)0)
#define ASSERT_DEFINED(expr) ((void)0)
#else
#define ASSERT(expr) \
    do { \
      if (!(expr)) { \
        std::fprintf(stderr, "Assertion failed: (%s), file %s, line %d\n", \
                     #expr, __FILE__, __LINE__); \
        DEBUG_BREAK(); \
        std::abort(); \
      } \
    } while (false)
#define ASSERT_INBOUNDS(idx, len) \
    do { \
      if (!(idx >= 0 && idx < len)) { \
        std::fprintf(stderr, "Assertion failed: (%s out of bounds), file %s, line %d\n", \
                     #idx, __FILE__, __LINE__); \
        DEBUG_BREAK(); \
        std::abort(); \
      } \
    } while (false)
#define ASSERT_DEFINED(var) \
    do { \
      if (!(var)) { \
        std::fprintf(stderr, "Assertion failed: (%s undefined), file %s, line %d\n", \
                     #var, __FILE__, __LINE__); \
        DEBUG_BREAK(); \
        std::abort(); \
      } \
    } while (false)
#endif