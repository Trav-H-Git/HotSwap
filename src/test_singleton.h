#pragma once
#ifdef XEN_CORE_EXPORT
#define XEN_CORE_API __declspec(dllexport)
#else
#define XEN_CORE_API __declspec(dllimport)
#endif

class XEN_CORE_API TestSingleton
{
private:
	static TestSingleton* m_instance;

public:
	TestSingleton();
	virtual ~TestSingleton();

	//int GetAndInc();
	static void InitInstance();
	static inline TestSingleton* GetInstance() { return m_instance; };
};
