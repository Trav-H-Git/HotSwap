:: @echo off
del xen_core.cpp 2>nul
:: del xen_core.h 2>nul
echo #include "xen_core.h" > xen_core.cpp
:: copy ..\..\..\src\editor\xen_core.h .
mkdir obj 2>nul
echo Building precompiled header...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
:: with xen_core.obj
::cl.exe /c /Yc"xen_core.h" /Fp"xen_core.pch" /Fo"obj\xen_core.obj" /I"..\..\src" /EHsc /MDd /std:c++20 /D"XEN_WINDOWS"  xen_core.cpp 
cl.exe /c /Yc"xen_core.h" /Fp"xen_core.pch" /Fo"obj\xen_core.obj" /EHsc /MDd /std:c++20 /D"XEN_WINDOWS" /showIncludes xen_core.cpp
:: without xen_core.obj
:: cl.exe /c /Yc"xen_core.h" /Fp"xen_core.pch" /I"..\..\src" /EHsc /MDd /std:c++20 /D"XEN_WINDOWS"
pause