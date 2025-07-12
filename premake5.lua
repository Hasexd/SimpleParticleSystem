workspace "SimpleParticleSystem"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "SimpleParticleSystem"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    IncludeDir = {}
    IncludeDir["GLFW"] = "vendor/glfw/include"
    IncludeDir["GLAD"] = "vendor/glad/include"
    IncludeDir["cimgui"] = "vendor/cimgui"
    
    include "vendor/glfw"
    include "vendor/cimgui"
    
    project "SimpleParticleSystem"
        kind "ConsoleApp"
        language "C"
        cdialect "C23"
        staticruntime "on"
        
        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
        
        files {
            "src/**.h",
            "src/**.c",
            "vendor/glad/src/glad.c"
        }
        
        includedirs {
            "src",
            "%{IncludeDir.GLFW}",
            "%{IncludeDir.GLAD}",
            "%{IncludeDir.cimgui}"
        }
        
        links {
            "cimgui",
            "GLFW",
            "opengl32.lib"
        }
         
        dependson {
            "GLFW",
            "cimgui"
        }
        
        vpaths{
            ["Header Files"] = {"src/**.h"},
            ["Source Files"] = {"src/**.c"},
        }
       
        filter "system:windows"
            systemversion "latest"
            
        filter "configurations:Debug"
            defines "DEBUG"
            runtime "Debug"
            symbols "on"
            
        filter "configurations:Release"
            defines "RELEASE"
            runtime "Release"
            optimize "on"