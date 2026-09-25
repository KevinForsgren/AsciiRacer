cmake_minimum_required(VERSION 4.3)
project(AsciiRacer)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(AsciiRacer src/main.cpp
        src/terminal.cpp
        src/header/terminal.h
        src/render.cpp
        src/header/render.h
        src/header/AsciiArt.h
        src/cars.cpp
        src/header/cars.h
        src/header/cars.h
        src/header/gameSettings.h
        src/environment.cpp
        src/header/environment.h
)

if (MSVC)
add_compile_options(/utf-8)
endif()