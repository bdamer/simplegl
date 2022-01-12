cmake_minimum_required(VERSION 2.8)
project(simplegl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Libraries
find_package(SDL2 REQUIRED)

find_package(GLEW REQUIRED)
# Set OpenGL preference
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

add_executable(simplegl simplegl.cpp)
target_link_libraries(simplegl ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
