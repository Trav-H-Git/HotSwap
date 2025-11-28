#include "test_node.h"
#include <cstdlib>
#include <windows.h>
#include <string>
//#include <fstream>
#include <iostream>
#include "test_singleton.h"
#include "xen_vector.h"

#include "app.h"
typedef TestNode* (*CreateFunc)();

//cl /EHsc /Zc:preprocessor /Zc:__cplusplus /permissive- /std:c++20 /W4 /WX /I include xen.cpp /link /out:xen.exe

//cl /EHsc /Zc:preprocessor /Zc:__cplusplus /permissive- /std:c++20 /W4 /WX /I include /Fp:pch.pch /c pch.cpp
//cl /EHsc /Zc:preprocessor /Zc:__cplusplus /permissive- /std:c++20 /W4 /WX /I include /Fp:pch.pch /Yu:pch.h xen.cpp /link /out:xen.exe

HMODULE HotSwapScripts(CreateFunc& outCreateFunc) 
{
    // Step 1: Write Derived.cpp if not already present (optional)
   /* std::ofstream derivedFile(derivedSourcePath);
    if (!derivedFile.is_open()) {
        std::cerr << "Failed to create Derived.cpp" << std::endl;
        return nullptr;
    }
    derivedFile << "#include \"core.h\"\n"
        << "class Derived : public AbstractBase {\n"
        << "public:\n"
        << "    void doSomething() override {\n"
        << "        // Runtime-built implementation\n"
        << "    }\n"
        << "};\n"
        << "extern \"C\" __declspec(dllexport) AbstractBase* createDerived() {\n"
        << "    return new Derived();\n"
        << "}\n";
    derivedFile.close();*/

    //  Build derived.dll using cl.exe
    //std::string clPath = "cl.exe"; // Adjust if not in PATH (e.g., full path to cl.exe)
    //std::string outputDll = "scripts.dll";
    //std::string compileCmd = "\"" + clPath + "\" /LD\"" + scriptSourcePath + "\" "
    //    "/Yu\"" + coreHPath + "\" /Fp\"" + corePchPath + "\" "
    //    "/Fe\"" + outputDll + "\" /link /LIBPATH:\"" + coreLibPath + "\" xen_core.lib";

    //std::string cl_path_cmd = "call \"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat\" x64";
    std::string cl_path = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.42.34433\\bin\\Hostx64\\x64\\cl.exe";
    // Execute the compiler command
   /* int result = system(cl_path_cmd.c_str());
    if (result != 0) {
        std::cerr << "Failed to set cl.exe path (error code: " << result << ")" << std::endl;
        return nullptr;
    }*/

  /*  std::string compileCmd = "\"\"" + cl_path + "\" /LD /MDd /EHsc /std:c++20 /D\"XEN_WINDOWS\" /I\"C:\\Engines\\HotSwap\" \"" + scriptSourcePath + "\" "
        "/Yu\"build\\bin\\xen_core.h\" /Fp\"build\\bin\\xen_core.pch\" /Fe\"scripts.dll\" "
        "/link /LIBPATH:\"build\\bin\" xen_core.lib \"obj\\xen_core.obj\"";*/

    /*std::string compileCmd = "call \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64 && cl.exe /LD /MDd /EHsc /std:c++20 /D\"XEN_WINDOWS\" /I\"C:\\Engines\\HotSwap\" \"" + scriptSourcePath + "\" "
        "/Yu\"build\\bin\\xen_core.h\" /Fp\"build\\bin\\xen_core.pch\" /Fe\"scripts.dll\" "
        "/link /LIBPATH:\"build\\bin\" xen_core.lib \"obj\\xen_core.obj\"";*/

    std::string command = "call \"\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64";
    command += " && cl.exe";
    command += " /LD";  // Create a DLL
    command += " /MDd";  // Use the debug DLL version of the runtime library
    command += " /EHsc";  // Enable C++ exceptions with standard calling convention
    command += " /std:c++20";  // Use C++20 standard
    command += " /D\"XEN_WINDOWS\"";  // Define the macro XEN_WINDOWS
    command += " \"script.cpp\"";  // Source file
    command += " /Yu\"xen_core.h\"";  // Use precompiled header xen_core.h
    command += " /Fp\"xen_core.pch\"";  // Name of the precompiled header file
    command += " /Fe\"scripts.dll\"";  // Name of the output DLL
    command += " /showIncludes";  // Show include files during compilation
    command += " /link";  // Linker options follow
    command += " /LIBPATH:\".\"";  // Add current directory to library search path
    command += " xen_core.lib";  // Link with xen_core.lib
    command += " \"obj\\xen_core.obj\"";  // Additional object file

    std::string cmd = "build_proj.bat";
    std::cout << "Executing: " << cmd << std::endl;
    //std::cin.get();
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Failed to build scripts.dll (error code: " << result << ")" << std::endl;
        return nullptr;
    }

    // Load the newly built derived.dll
    HMODULE scriptDll = LoadLibraryA("scripts.dll");
    if (!scriptDll) {
        std::cerr << "Failed to load scripts.dll: " << GetLastError() << std::endl;
        return nullptr;
    }

    //  Get the factory function
    outCreateFunc = (CreateFunc)GetProcAddress(scriptDll, "Create");
    if (!outCreateFunc) {
        std::cerr << "Failed to get Create() from scripts.dll: " << GetLastError() << std::endl;
        FreeLibrary(scriptDll);
        return nullptr;
    }

    std::cout << "Successfully built and loaded derived.dll" << std::endl;
    return scriptDll; // Caller is responsible for freeing this later
}

//int main()
//{
//
// //   Vector<int> vec1;
// //   vec1.PushBack(3);
// //   vec1.Print();
// //   TestSingleton ts;
// //   std::cout << ts.GetAndInc() << std::endl;
// //   std::cout << ts.GetInstance()->GetAndInc() << std::endl;
// //  
//
//	//TestNode test_node = TestNode{};
//	//test_node.Test();
//	//std::cin.get();
// //  
//
// //   // Build and load scripts.dll
// //   CreateFunc create_func = nullptr;
// //   HMODULE scriptDll = HotSwapScripts(create_func);
// //   if (!scriptDll) {
// //       //FreeLibrary(coreDll);
// //       std::cerr << "Failed to hot swap scripts.dll" << std::endl;
// //       return 1;
// //   }
//
// //   // Use the derived object
// //   TestNode* script = (TestNode*)create_func();
// //   script->Test();
//
//
// //   std::cout << "paused for \"re-Test()\"" << std::endl;
//	//std::cin.get();
// //   delete script;  // Free the Script object
// //   script = nullptr;
// //   create_func = nullptr;
// //   FreeLibrary(scriptDll);
// //   scriptDll = HotSwapScripts(create_func);
// //   if (!scriptDll) {
// //       //FreeLibrary(coreDll);
// //       std::cerr << "Failed to hot swap scripts.dll on the second test" << std::endl;
//	//    std::cin.get();
// //       return 1;
// //   }
//
// //   script = (TestNode*)create_func();
// //   script->Test(); 
// //   // Cleanup
// //   delete script;
// //   FreeLibrary(scriptDll);
// //   //FreeLibrary(coreDll);
//	//std::cin.get();
//
//	return 0;
//}



int main() {
   
    App::StartUp();
    App::TestVector();
    App::Shutdown();
    return 0;
}