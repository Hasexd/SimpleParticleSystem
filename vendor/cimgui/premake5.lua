function getBackendsFolder()
    if os.isdir("imgui/backends") then
        return "imgui/backends/"
    else
        return "imgui/examples/"
    end
end

function getTablesSource()
    if os.isfile("imgui/imgui_tables.cpp") then
        return "imgui/imgui_tables.cpp"
    else
        return nil
    end
end


project "cimgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "on"
    
    targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")
    
    files {
        "cimgui.cpp",
        "cimgui_impl.cpp",
        "imgui/imgui.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_widgets.cpp"
    }
    
    -- Add tables source if it exists
    local tablesSource = getTablesSource()
    if tablesSource then
        files { tablesSource }
    end
    
    -- Add backend files
    local backendsFolder = getBackendsFolder()
    files {
        backendsFolder .. "imgui_impl_opengl3.cpp",
        backendsFolder .. "imgui_impl_glfw.cpp"
    }
    
    includedirs {
        "",
        "imgui",
        "../glfw/include"  -- GLFW headers
    }
    
    defines {
        "IMGUI_USER_CONFIG=\"../cimconfig.h\"",
        "IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1",
        "IMGUI_IMPL_OPENGL_LOADER_GL3W"
    }
    filter "system:windows"
        defines {
            "IMGUI_IMPL_API=extern \"C\" __declspec(dllexport)"
        }
        
    filter "system:not windows"
        defines {
            "IMGUI_IMPL_API=extern \"C\" "
        }
    filter {}
    
    -- Optional FreeType support
    -- Uncomment and modify paths as needed
    --[[
    filter "options:freetype"
        files {
            "imgui/misc/freetype/imgui_freetype.cpp"
        }
        defines {
            "IMGUI_ENABLE_FREETYPE=1",
            "IMGUI_ENABLE_STB_TRUETYPE=1"
        }
        links {
            "freetype"
        }
    filter {}
    -]]