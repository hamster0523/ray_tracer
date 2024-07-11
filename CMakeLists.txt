cmake_minimum_required(VERSION 3.15)
project(ray_tracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

add_executable(main main.cpp)

# add_subdirectory(material)

# target_link_libraries(main PUBLIC material)