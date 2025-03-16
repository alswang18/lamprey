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
add_requires("assimp", {configs = {runtimes = is_mode("debug") and "MTd" or "MT", 
shared = false}})

-- Define target
target("lamprey")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
    add_includedirs("src")
    add_includedirs("res")

    add_files("src/Bindable/*.cpp")
    add_headerfiles("src/Bindable/*.h")
    add_includedirs("src/Bindable")
    
    add_headerfiles("src/Macros/*.h")
    add_includedirs("src/Macros")
    
    add_files("src/Drawable/*.cpp")
    add_headerfiles("src/Drawable/*.h")
    add_includedirs("src/Drawable")

    add_headerfiles("src/Geometry/*.h")
    add_includedirs("src/Geometry")


    add_files("src/imgui/*.cpp")
    add_headerfiles("src/imgui/*.h")
    add_includedirs("src/imgui")

    add_defines("IMGUI_IMPL_WIN32_DISABLE_GAMEPAD")

    add_packages("assimp")


    add_packages("fmt")
    -- MSVC specific flags
    if is_plat("windows") then
        add_files("res/app.rc")
        add_headerfiles("res/resource.h")

        -- Remove default warning level and set our own
        set_warnings("all")     -- This removes default warning flags
        add_cxxflags("/W3", {force = true})
        add_syslinks("user32", "gdi32")  -- Add Windows system libraries
        add_links("gdiplus")

        
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

    add_files("shaders/*.hlsl", {rule = "compile_shaders"})
    add_files("images/*.png" , {rule = "copy_images"})

    -- we don't directly use obj as a filetype as xmake gets confused when the file ends in .obj
    add_files("models/*.obj.model" , {rule = "copy_obj_models"})
    -- Copy the compiled shader to the output directory
    after_build(function (target)
        local outputdir = target:targetdir()
        os.mkdir(outputdir)
        for _, file in ipairs(os.files(path.join(target:objectdir(), "shaders") .. "/*")) do
            os.cp(file, outputdir)
        end

        for _, file in ipairs(os.files(path.join(target:objectdir(), "images") .. "/*")) do
            os.cp(file, outputdir)
        end

        for _, file in ipairs(os.files(path.join(target:objectdir(), "models") .. "/*")) do
            os.cp(file, outputdir)
        end
    end)

rule("compile_shaders")
    set_extensions(".hlsl")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        -- Only process VertexShader.hlsl
        if not sourcefile:endswith(".hlsl") then
            print(sourcefile)
            return
        end
        
        
        -- Get output file (same name but .cso extension)
        local outputdir = target:objectdir()
        local outputfile = sourcefile:gsub(".hlsl", ".cso")
        local outputfile = path.join(outputdir, outputfile)
        
        -- Ensure output directory exists
        batchcmds:mkdir(path.join(outputdir, "shaders"))
        
        if sourcefile:endswith("VS.hlsl") then
            batchcmds:execv("fxc", {"/T", "vs_5_0", "/E", "main", "/Fo", outputfile, sourcefile})
        elseif sourcefile:endswith("PS.hlsl") then
            batchcmds:execv("fxc", {"/T", "ps_5_0", "/E", "main", "/Fo", outputfile, sourcefile})
        else 
            print("failed: " .. sourcefile .. " is not a valid shader name")
            return
        end


        -- Add the output file to the target
        batchcmds:add_depfiles(sourcefile)
        batchcmds:set_depmtime(os.mtime(outputfile))
        batchcmds:set_depcache(target:dependfile(outputfile))
        
        -- Return the output file
        return outputfile
    end)

rule("copy_images")
    set_extensions(".png")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
    
    batchcmds:mkdir(path.join(target:objectdir(), "images"))
    local outputdir = target:objectdir()
    local outputfile = path.join(outputdir, sourcefile)

    batchcmds:add_depfiles(sourcefile)
    batchcmds:set_depmtime(os.mtime(outputfile))
    batchcmds:set_depcache(target:dependfile(outputfile))
    batchcmds:cp(sourcefile, outputfile)


    return outputfile

    end)

rule("copy_obj_models")
    set_extensions(".obj.model")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
    
    batchcmds:mkdir(path.join(target:objectdir(), "models"))
    local outputdir = target:objectdir()
    local outputfile = path.join(outputdir, sourcefile)

    print(outputfile)

    batchcmds:cp(sourcefile, outputfile)

    return

    end)