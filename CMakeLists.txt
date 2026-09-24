cmake_minimum_required(VERSION 4.3)
project(AsciiRacer)

set(CMAKE_CXX_STANDARD 23)

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