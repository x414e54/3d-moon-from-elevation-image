cmake_minimum_required(VERSION 2.6)
project(moon)

include_directories(${CMAKE_SOURCE_DIRECTORY}/libs/glm)
add_executable(moon moon.cpp icosphere.cpp opengl_renderer.cpp vulkan_renderer.cpp)
target_link_libraries(moon GL SDL2) 
