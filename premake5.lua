-- premake5.lua
workspace "Xen"
   architecture "x86_64"
   configurations { "Debug", "Release", "Dist" }
   startproject "xen_runtime"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
   buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

--OutputDir = "%{prj.name}-%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"


--group "Core"
--include "SVEngine"
--include "SVEditor"
--group ""

------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------

project "xen_core"
   kind "SharedLib"
   language "C++"
   cppdialect "C++20"

   targetdir ( "build/bin" )
   objdir ("build/obj" )

   files 
   { 
        "%{wks.location}/src/**.h",
        "%{wks.location}/src/**.c",
        "%{wks.location}/src/**.hpp",
        "%{wks.location}/src/**.cpp" 
    }

    removefiles 
    { 
        "%{wks.location}/src/main.cpp",
       
    }

   includedirs
   {
       
       "%{wks.location}/src"
    --    "%{wks.location}/src/core",
    --    "%{wks.location}/src/core/types",
    --    "%{wks.location}/src/core/scene",
    --    "%{wks.location}/src/platform",
    --    "%{wks.location}/src/mediator",
    --    "%{wks.location}/src/dependency/glad/include",
    --    "%{wks.location}/src/core/input",
    --    "%{wks.location}/src/core/input/event",
    --    "%{wks.location}/src/core/memory",
    --    "%{wks.location}/src/object",
    --    "%{wks.location}/src/object/node",
    --    "%{wks.location}/src/object/resource"
       --"%{wks.location}/src/dependency/glfw/include"
   }

--    links
--    {
--       "glad",
--       "opengl32"
--       --"glfw"
--    }

   defines { "XEN_CORE_EXPORT" }

   filter "system:windows"
       systemversion "latest"
       defines { "XEN_WINDOWS" }

    filter "system:linux"
       systemversion "latest"
       defines { "XEN_LINUX" }

   filter "configurations:Debug"
       defines { "XEN_DEBUG" }
       runtime "Debug"
       staticruntime "off"
       symbols "On"

   filter "configurations:Release"
       defines { "XEN_RELEASE" }
       staticruntime "on"
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "XEN_DIST" }
       staticruntime "on"
       runtime "Release"
       optimize "On"
       symbols "Off"

---------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------

project "xen_runtime"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   targetdir ( "build/bin" )
   objdir ("build/obj" )


    includedirs
    {
        "%{wks.location}/src"
       
    }

    --libdirs { "build/obj/" .. OutputDir .. "/" }

    postbuildcommands
    {
       --'{COPY} build/bin/xen_core-%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/xen_core.dll %{cfg.targetdir}'
       --"cl /P path\\to\\header.h /Fi:path\\to\\output.i"

       -- create xen core PCH in output directory
       "{COPY} src/xen_core.h build/bin",
       "{COPY} src/test_node.h build/bin",
       "{COPY} src/test_node.cpp build/bin",
       "{COPY} src/test_singleton.h build/bin",
       "{COPY} src/test_singleton.cpp build/bin",
       "{COPY} src/main.cpp build/bin",
       "{COPY} src/xen_vector.h build/bin",
       "{COPY} src/xen_vector.cpp build/bin"
       --'cl.exe /Ycxen_core.h /Fp"build/bin/"' .. OutputDir .. '/ /EHsc /MD /std:c++20 /D"XEN_WINDOWS" /I"src/core/" /Fo"obj/%{cfg.buildcfg}/"'
    }

    links
    {
        "xen_core"
    }



   filter "system:windows"
       systemversion "latest"
       defines { "XEN_WINDOWS" }
       files 
       { 
           "%{wks.location}/src/main.cpp"
       }       
 

    filter "system:linux"
       systemversion "latest"
       defines { "XEN_LINUX" }
       files 
       { 
           "%{wks.location}/src/main.cpp"
       }

   filter "configurations:Debug"
       defines { "XEN_DEBUG" }
       staticruntime "off"
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "XEN_RELEASE" }
       staticruntime "on"
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "XEN_DIST" }
       staticruntime "on"
       runtime "Release"
       optimize "On"
       symbols "Off"

