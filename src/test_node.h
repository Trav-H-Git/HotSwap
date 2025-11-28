#pragma once

#ifdef XEN_CORE_EXPORT
#define XEN_CORE_API __declspec(dllexport)
#else
#define XEN_CORE_API __declspec(dllimport)
#endif
#include <iostream>


class XEN_CORE_API TestNode
{
public: 
	virtual void Test();
private:
	TestNode* _parent = nullptr;


public:
	TestNode();
	virtual ~TestNode();

};

