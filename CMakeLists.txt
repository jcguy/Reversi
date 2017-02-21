cmake_minimum_required(VERSION 3.7)
project(reversi)

set(CMAKE_CXX_STANDARD 11)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})


set(SOURCE_FILES reversi.cpp)
add_executable(reversi.out ${SOURCE_FILES})

target_link_libraries(reversi.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
