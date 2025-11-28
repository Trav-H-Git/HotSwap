#include "test_singleton.h"

TestSingleton* TestSingleton::m_instance = nullptr;

TestSingleton::TestSingleton()
{
	//m_instance = this;
}

TestSingleton::~TestSingleton()
{
}


void TestSingleton::InitInstance()
{
	m_instance = new TestSingleton();
}
