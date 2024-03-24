cmake_minimum_required(VERSION 3.24)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions("GLM_ENABLE_EXPERIMENTAL")

set(CUSTOM_FLAGS "-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wmost -Wmove -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wextra-semi -Wfloat-equal -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CUSTOM_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG -g)
set(CMAKE_CXX_FLAGS_RELEASE -O3)

set(GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/generated_files)

include(cmake/embed.cmake)

file(GLOB_RECURSE SOURCES main.cpp core/*.cpp lessons/*.cpp)
add_executable(run ${SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

target_include_directories(run PRIVATE . ${GENERATED_FILES})
add_dependencies(run IMGResources)

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIR})
    target_link_libraries(run PRIVATE ${GLEW_LIBRARIES})
endif()

find_package(GLFW3 REQUIRED)
if (GLFW3_FOUND)
    include_directories(${GLFW3_INCLUDE_DIR})
    target_link_libraries(run PRIVATE ${GLFW3_LIBRARY})
endif()

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    add_definitions(${OPENGL_DEFINITIONS})
    target_link_libraries(run PRIVATE ${OPENGL_LIBRARIES})
endif()

find_package(SOIL REQUIRED)
if (SOIL_FOUND)
    include_directories(${SOIL_INCLUDE_DIR})
    target_link_libraries(run PRIVATE ${SOIL_LIBRARY})
endif()
