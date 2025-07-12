project "GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "include/GLFW/**.h",
        "src/**.c"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines
        {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"