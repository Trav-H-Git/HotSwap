#include "build/bin/xen_core.h"
#include "script.h"

Script::Script()
{

}
Script::~Script()
{
}

void Script::Test()
{
    //std::cout << "SCRIPT! Test()" << std::endl;
    return;
}
extern "C" __declspec(dllexport) TestNode* Create()
{
    return new Script();
}
