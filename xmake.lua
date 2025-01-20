set_project("lamprey")
set_version("0.1.0")

-- Modern C++ settings
set_languages("c++20")

-- Set MSVC as the toolchain
if is_host("windows") then
    set_toolchains("msvc")
end

add_requires("glfw")                    -- Latest stable version

add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})


-- Define target
target("lamprey")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
    add_includedirs("src")

    add_packages("glfw")

    -- MSVC specific flags
    if is_plat("windows") then
        -- Remove default warning level and set our own
        set_warnings("all")     -- This removes default warning flags
        add_cxxflags("/W3", "/WX", {force = true})
        
        -- Debug configuration
        if is_mode("debug") then
            set_symbols("debug")
            add_cxxflags("/Zi", "/FS")  -- Generate complete debugging information
            add_ldflags("/DEBUG:FULL")   -- Include all debug info
            set_runtimes("MDd")         -- Debug runtime
        end
        
        -- Optimize for performance in release mode
        if is_mode("release") then
            add_cxxflags("/O2", "/Oi", "/Ot")
            add_cxxflags("/GL")  -- Whole program optimization
            add_ldflags("/LTCG") -- Link-time code generation
        end
        
        -- Enable additional security features
        add_cxxflags("/GS")     -- Buffer security check
        add_cxxflags("/guard:cf") -- Control Flow Guard
        
        -- Modern C++ conformance
        add_cxxflags("/permissive-") -- Strict C++ conformance
        add_cxxflags("/Zc:__cplusplus") -- Correct __cplusplus macro
        add_cxxflags("/Zc:preprocessor") -- Conforming preprocessor
    end