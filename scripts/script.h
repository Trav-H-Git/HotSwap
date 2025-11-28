#pragma once
#include "build/bin/xen_core.h"

class Script : public TestNode
{
public: 
	void Test() override;
private:
	Script* _parent = nullptr;


public:
	Script();
	virtual ~Script();

};

