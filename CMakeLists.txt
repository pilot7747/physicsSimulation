cmake_minimum_required(VERSION 3.15)
project(physics_simulation)
set(CMAKE_CXX_STANDARD 17)

add_executable(physics_simulation main.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

target_link_libraries(physics_simulation ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )