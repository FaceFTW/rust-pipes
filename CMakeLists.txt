# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project("gl-pipes")

#file(SRC_FILE
#${CMAKE_SOURCE_DIR})

# Add source to this project's executable.

# Do Lib Stuff
# OpenGL
FIND_PACKAGE(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR "OpenGL not found!")
endif(NOT OPENGL_FOUND)
message(STATUS "Found OpenGL Base")


if(NOT OpenGL_GLX_FOUND)
		message(ERROR "OpenGL GLX not found!")
endif(NOT OpenGL_GLX_FOUND)
message(STATUS "Found OpenGL GLX")

find_package(GLUT REQUIRED)
if(NOT GLUT_FOUND)
		message(ERROR "GLUT not found!")
endif(NOT GLUT_FOUND)
message(STATUS "Found GLUT")

find_package(X11)



add_library(OpenGL::OpenGL STATIC IMPORTED)
#add_library(OpenGL::GLX STATIC IMPORTED)
#add_library(GLUT::GLUT STATIC IMPORTED)
#add_library(X11::X11 STATIC IMPORTED)
set(LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${X11_X11_LIB})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${X11_INCLUDE_DIR})
LINK_LIBRARIES(${LIBS})

add_executable (gl-pipes "pipe_test.c")
TARGET_LINK_LIBRARIES(gl-pipes ${LIBS})
