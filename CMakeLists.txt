cmake_minimum_required(VERSION 3.26)
project(MathEvaluator C)

set(CMAKE_C_STANDARD 11)

set(SDL2_DIR ${CMAKE_SOURCE_DIR}/SDL)
add_subdirectory(${SDL2_DIR})

# Set options to ensure SDL is built as a static library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libraries" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Disable building SDL as a shared library" FORCE)

add_executable(MathEvaluator
        lexer.h
        solver.h
        tokenArrayTools.h
        base.h
        inputFlags.h
        graphSolver.h
        doubleTools.h
        help.h
        constants.h
        main.c
        graph.h
        colour.h
        windowIcon.h
        terminal.h
)

if(WIN32)
    target_link_libraries(MathEvaluator PRIVATE mingw32 SDL2main SDL2-static m)
    target_include_directories(MathEvaluator PRIVATE ${SDL2_DIR}/include)
elseif(UNIX)
    target_link_libraries(MathEvaluator PRIVATE readline SDL2main SDL2-static m)
    target_include_directories(MathEvaluator PRIVATE ${SDL2_DIR}/include /usr/include/readline)
endif()