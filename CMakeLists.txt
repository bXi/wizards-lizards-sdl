cmake_minimum_required(VERSION 3.22)

project("WizardsAndLizards")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)


enable_language(CXX)

set(CMAKE_C_STANDARD 23)

if (EMSCRIPTEN)
    message("EMSCRIPTEN BUILD!!!")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -msimd128 -s TOTAL_MEMORY=67108864 -sUSE_SDL=3 -sUSE_SDL_MIXER=3 -s ASSERTIONS=1 -s WASM=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -msimd128 -s TOTAL_MEMORY=67108864 -sUSE_SDL=3 -sUSE_SDL_MIXER=3 -s ASSERTIONS=1 -s WASM=1")
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "-O3 -march=x86-64-v3")
        set(CMAKE_C_FLAGS "-O3 -march=x86-64-v3")
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
        add_definitions(-DSDL_DISABLE_LOCKS)
        set(CMAKE_EXE_LINKER_FLAGS "-static -mwindows")
    else ()
        set(CMAKE_CXX_FLAGS "-march=x86-64-v3")
        set(CMAKE_C_FLAGS "-march=x86-64-v3")
        set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif ()
endif ()


add_subdirectory(libs)
add_subdirectory(src)