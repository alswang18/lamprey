set_project("lamprey")
set_version("0.1.0")

if not is_host("windows") then
    print("Error: Platform not supported")
    print("This project currently only supports Windows")
    -- exit early
    return
end

-- Modern C++ settings
set_languages("c++20")

-- Set MSVC as the toolchain
if is_host("windows") then
    set_toolchains("msvc")
    -- This helps clangd understand MSVC-specific code
    add_cxflags("/permissive-")
    
    -- Help with MSVC compatibility
    add_defines("_CRT_SECURE_NO_WARNINGS")
    if is_mode("vs") then
        set_configvar("VS_USE_SOLUTION_FOLDERS", true)
        set_policy("vs.solution.project.use_script_file_system", true)
    end
end

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})
add_rules("plugin.vsxmake.autoupdate")

after_build(function (target)
    os.setenv("VS_STARTUP_PROJECT", target:name())
    local vsxmake = target:pkg("vsxmake")
    if vsxmake then
        vsxmake.folder = "src"
    end
end)

add_requires("fmt", {configs = {runtimes = is_mode("debug") and "MTd" or "MT"}})

-- Define target
target("lamprey")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
    add_includedirs("src")
    add_includedirs("res")

    add_packages("fmt")
    -- MSVC specific flags
    if is_plat("windows") then
        add_files("res/app.rc")
        add_headerfiles("res/resource.h")

        -- Remove default warning level and set our own
        set_warnings("all")     -- This removes default warning flags
        add_cxxflags("/W3", {force = true})
        add_syslinks("user32", "gdi32")  -- Add Windows system libraries

        
        -- Debug configuration
        if is_mode("debug") then
            set_symbols("debug")
            add_cxxflags("/Zi", "/FS")  -- Generate complete debugging information
            add_ldflags("/DEBUG:FULL")   -- Include all debug info
            set_runtimes("MTd")
        end
        
        -- Optimize for performance in release mode
        if is_mode("release") then
            add_cxxflags("/O2", "/Oi", "/Ot")
            add_cxxflags("/GL")  -- Whole program optimization
            set_runtimes("MT")
            add_defines("NDEBUG")
        end
        
        -- Enable additional security features
        add_cxxflags("/GS")     -- Buffer security check
        add_cxxflags("/guard:cf") -- Control Flow Guard
        
        -- Modern C++ conformance
        add_cxxflags("/permissive-") -- Strict C++ conformance
        add_cxxflags("/Zc:__cplusplus") -- Correct __cplusplus macro
        add_cxxflags("/Zc:preprocessor") -- Conforming preprocessor
   
    end
    -- Add shader directory definition
    add_defines("SHADER_DIR=\"shaders\"")
    
    -- Link shader management to this target
    add_deps("shaders")

-- Separate target just for shader management
target("shaders")
    set_kind("phony") -- Not an actual binary output
    
    -- Track shader files for IDE integration
    add_headerfiles("shader/*.hlsl")
    
    -- Custom build step to copy shaders
    on_build(function (target)
        -- Get the paths
        local shader_source_dir = path.join(target:scriptdir(), "shader")
        local shader_target_dir = path.join("$(buildir)", "shaders")
        
        -- Create output directory
        os.mkdir(shader_target_dir)
        
        -- Copy all shader files
        print("Copying shader files to " .. shader_target_dir)
        os.cp(path.join(shader_source_dir, "*.hlsl"), shader_target_dir)
        
        -- Print result with a count of files
        local shader_files = os.files(path.join(shader_source_dir, "*.hlsl"))
        print(string.format("Copied %d shader files", #shader_files))
    end)
    
    -- On install, copy shaders to the install directory too
    on_install(function (target) 
        local shader_source_dir = path.join(target:scriptdir(), "shader")
        local shader_install_dir = path.join("$(installdir)", "shaders")
        
        os.mkdir(shader_install_dir)
        os.cp(path.join(shader_source_dir, "*.hlsl"), shader_install_dir)
    end)