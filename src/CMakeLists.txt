cmake_minimum_required (VERSION 2.6)

project (Vii)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(stdc++fs UNKNOWN IMPORTED)
set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")

FILE(GLOB sources *.cpp *.h)
add_compile_options("-std=c++11")

add_executable(Vii ${sources})
target_link_libraries(${PROJECT_NAME} stdc++fs)