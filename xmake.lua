add_rules("mode.debug", "mode.release")

add_includedirs("include/")

set_languages("c++20")

add_requires("gtest","openssl")

target("hw2")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("gtest","openssl")
    set_rundir("$(projectdir)")