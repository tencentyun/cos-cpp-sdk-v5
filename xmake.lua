add_rules("mode.debug", "mode.release")

local PocoDebug = false
if is_mode("debug") then
    PocoDebug = true
end

add_requires("poco", {configs = {json = true, net = true, netssl = true, crypto = true, util = true, debug = PocoDebug } })
add_requireconfs("poco.expat", {override = true, system = false})

set_languages("c++17")

set_encodings("utf-8")

-- add_defines("POCO_STATIC")

target("cossdk")
    set_kind("shared")
    add_files("src/**.cpp")
    add_includedirs("./include", {public = true})
    add_headerfiles("./include/**.h")
    add_defines("USE_OPENSSL_MD5")
    add_packages("poco", {public = true})
    if is_plat("windows") then
        add_rules("utils.symbols.export_all", {export_classes = true})
    end

target("get_object_demo")
    set_kind("binary")
    add_files("demo/object_op_demo/get_object_demo.cpp")
    add_deps("cossdk")

target("put_object_demo")
    set_kind("binary")
    add_files("demo/object_op_demo/put_object_demo.cpp")
    add_deps("cossdk")
