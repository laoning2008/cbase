set_project("cbase")
set_languages("c99", "c++17")
add_rules("mode.debug", "mode.release")

add_defines("_WIN32_WINNT=0x0501")

target("cbase")
    set_kind("static")
    add_files("base/**.cc")
    add_headerfiles("base/**.h")
    add_includedirs(".")
    add_includedirs(".", {public = true})
    add_filegroups("base", {rootdir = "base", files = {"**.*"}})

target("exam")
    set_kind("binary")
    add_files("exam/**.cpp")
    add_deps("cbase")