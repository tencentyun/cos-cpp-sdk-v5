


add_rules("mode.debug", "mode.release")

add_requires("poco", {configs = {json = true, net = true, netssl = true, crypto = true, util = true, shared = true}})
add_requireconfs("poco.expat", {override = true, system = false})


target("cossdk")
    set_kind("shared")
    set_languages("cxx17")
    add_files("src/**.cpp")
    add_includedirs("./include")
    add_headerfiles("./include/**.h")
    add_packages("poco")


target("get_object_demo")
    set_kind("binary")
    set_languages("cxx17")
    add_files("demo/object_op_demo/get_object_demo.cpp")
    add_includedirs("./include")
    add_headerfiles("./include/**.h")
    add_packages("poco")
    add_deps("cossdk")



target("put_object_demo")
    set_kind("binary")
    set_languages("cxx17")
    add_files("demo/object_op_demo/put_object_demo.cpp")
    add_includedirs("./include")
    add_headerfiles("./include/**.h")
    add_packages("poco")
    add_deps("cossdk")




