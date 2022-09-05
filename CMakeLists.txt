cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0072 NEW)
project("gl-pipes")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Add source to this project's executable.

# ########SOURCE/HEADER LISTS#########
set(GLAD_FILES
	${CMAKE_SOURCE_DIR}/src/gl.c
	${CMAKE_SOURCE_DIR}/src/include/glad/gl.h
	${CMAKE_SOURCE_DIR}/src/include/KHR/khrplatform.h
)

set(GL_PIPES_SOURCE

	${CMAKE_SOURCE_DIR}/src/utils.cpp
	${CMAKE_SOURCE_DIR}/src/pipe_layer.cpp
	${CMAKE_SOURCE_DIR}/src/pipe/pipe.cpp
	${CMAKE_SOURCE_DIR}/src/pipe/pipe_part.cpp
	${CMAKE_SOURCE_DIR}/src/pipe/pipe_draw.cpp
	${CMAKE_SOURCE_DIR}/src/bootstrap.cpp
	${CMAKE_SOURCE_DIR}/src/gl.c
	${CMAKE_SOURCE_DIR}
)

set(HEADER_FILES

	${CMAKE_SOURCE_DIR}/include/utils.h;
	${CMAKE_SOURCE_DIR}/include/pipe.h;
	${CMAKE_SOURCE_DIR}/include/pipe_layer.h;
	${CMAKE_SOURCE_DIR}/include/graphics/gl_drawThread.h
	${CMAKE_SOURCE_DIR}/include/draw.h
	${CMAKE_SOURCE_DIR}/src/include/glad/gl.h
	${CMAKE_SOURCE_DIR}/src/include/KHR/khrplatform.h
)

# include_directories(AFTER ${CMAKE_SOURCE_DIR}/include/*)

# #######FIND LIBRARIES#######
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

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# add_library(OpenGL::OpenGL STATIC IMPORTED)
# add_library(OpenGL::GLX STATIC IMPORTED)
# add_library(GLUT::GLUT STATIC IMPORTED)
# add_library(X11::X11 STATIC IMPORTED)
set(LIBS ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})
LINK_LIBRARIES(${LIBS})

add_executable(glscratchwork ${CMAKE_SOURCE_DIR}/src/scratch/main.cpp ${GLAD_FILES})
target_link_libraries(glscratchwork ${LIBS})

add_executable(gl-pipes ${GL_PIPES_SOURCE})
TARGET_LINK_LIBRARIES(gl-pipes ${LIBS})

# add_executable(gl-pipes-test "src/scratchwork/pipe_test_nox11.cpp")
# TARGET_LINK_LIBRARIES(gl-pipes-test ${LIBS})

# find_package(X11)
# set(LIBS ${LIBS} ${X11_X11_LIB})
# INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
# add_executable(gl-pipes-x11-test "src/scratchwork/pipe_test.c")

# TARGET_LINK_LIBRARIES(gl-pipes-x11-test ${LIBS})
