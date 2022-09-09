cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0072 NEW)
project("gl-pipes")

# ########SOURCE/HEADER LISTS#########
set(GLAD_FILES
	${CMAKE_SOURCE_DIR}/src/glad.c
	${CMAKE_SOURCE_DIR}/src/include/glad/glad.h
	${CMAKE_SOURCE_DIR}/src/include/KHR/khrplatform.h
)

set(GL_PIPES_SOURCE
	${CMAKE_SOURCE_DIR}/src/utils.cpp
	${CMAKE_SOURCE_DIR}/src/pipe_layer.cpp
	${CMAKE_SOURCE_DIR}/src/nodes.cpp
	${CMAKE_SOURCE_DIR}/src/pipelist.cpp
)

set(HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/include/utils.h;
	${CMAKE_SOURCE_DIR}/src/include/pipe_layer.h;
)

# #######FIND LIBRARIES#######
FIND_PACKAGE(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
	message(ERROR "OpenGL not found!")
endif(NOT OPENGL_FOUND)

message(STATUS "Found OpenGL Base")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(LIBS ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${CMAKE_DL_LIBS})
include_directories(${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})
LINK_LIBRARIES(${LIBS})

add_executable(glscratchwork ${CMAKE_SOURCE_DIR}/src/scratch/main.cpp ${GLAD_FILES})
target_link_libraries(glscratchwork ${LIBS})

add_executable(gl-pipes ${GL_PIPES_SOURCE} ${HEADER_FILES} ${GLAD_FILES})
TARGET_LINK_LIBRARIES(gl-pipes ${LIBS})
