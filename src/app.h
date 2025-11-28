#pragma once

#ifdef XEN_CORE_EXPORT
#define XEN_CORE_API __declspec(dllexport)
#else
#define XEN_CORE_API __declspec(dllimport)
#endif
#include "mem.h"
#include "xen_string.h"
#include "xen_primitives.h"



enum class Threads : u32
{
	Main = 0,
	Physics = 1,
	Count = 2
};
class XEN_CORE_API App
{
public:
	static bool StartUp();

    //test vector
    static bool TestVector();

	static bool Shutdown();
};

