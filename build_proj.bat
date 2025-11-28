@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
:: Runtime Build
cl.exe /LD /MDd /EHsc /std:c++20 /D"XEN_WINDOWS" /I"C:\Engines\HotSwap" "scripts\script.cpp" /Yu"build\bin\xen_core.h" /Fp"build\bin\xen_core.pch" /Fe"scripts.dll" /link /LIBPATH:"build\bin" xen_core.lib "build\bin\obj\xen_core.obj"

:: "cl.exe" /LD /MDd /EHsc /std:c++20 /D"XEN_WINDOWS" /I"C:\Engines\HotSwap" "scripts\script.cpp" /Yu"build\bin\xen_core.h" /Fp"build\bin\xen_core.pch" /Fe"scripts.dll" /link /LIBPATH:"build\bin" xen_core.lib
:: "cl.exe" /LD /MDd /EHsc /std:c++20 /D"XEN_WINDOWS" "scripts\script.cpp" /Yu"build\bin\xen_core.h" /Fp"build\bin\xen_core.pch" /Fe"scripts.dll" /link /LIBPATH:"build\bin" xen_core.lib
:: cl.exe /c /EHsc /MD /std:c++20 /D"XEN_SCRIPTS_EXPORT" /I"temp" /Yu"xen_core.h" /Fptemp/xen_core.pch *.cpp
:: link.exe xen_proj.obj xen_core.lib /DLL /OUT:xen_proj.dll



pause