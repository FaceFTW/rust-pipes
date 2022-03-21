# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project("gl-pipes")

#file(SRC_FILE
#${CMAKE_SOURCE_DIR})

# Add source to this project's executable.

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/graphics/*.c
	${CMAKE_SOURCE_DIR}/src/graphics/*.cpp)
	
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp
	${CMAKE_SOURCE_DIR}/src/graphics/*.h
	${CMAKE_SOURCE_DIR}/src/graphics/*.hpp
	)

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


add_library(OpenGL::OpenGL STATIC IMPORTED)
#add_library(OpenGL::GLX STATIC IMPORTED)
#add_library(GLUT::GLUT STATIC IMPORTED)
#add_library(X11::X11 STATIC IMPORTED)
set(LIBS ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
LINK_LIBRARIES(${LIBS})

add_executable(gl-pipes ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(gl-pipes ${LIBS})



add_executable(gl-pipes-test "src/scratchwork/pipe_test_nox11.cpp")
TARGET_LINK_LIBRARIES(gl-pipes-test ${LIBS})


find_package(X11)
set(LIBS ${LIBS} ${X11_X11_LIB})
INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
add_executable (gl-pipes-x11-test "src/scratchwork/pipe_test.c")

TARGET_LINK_LIBRARIES(gl-pipes-x11-test ${LIBS})
