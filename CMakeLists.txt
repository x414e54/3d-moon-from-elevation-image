cmake_minimum_required(VERSION 2.6)
project(moon)

add_executable(moon moon.cpp opengl_renderer.cpp)
target_link_libraries(moon GL SDL2) 
