cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0072 NEW)
project("gl-pipes")


set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" )
# set(CMAKE_CXX_STANDARD_REQUIRED 11)
# ########SOURCE/HEADER LISTS#########

set(PCG_FILES
	${CMAKE_SOURCE_DIR}/pcg/pcg_basic.c
	${CMAKE_SOURCE_DIR}/pcg/pcg_basic.h
)

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
	${CMAKE_SOURCE_DIR}/src/include/utils.h
	${CMAKE_SOURCE_DIR}/src/include/nodes.h
	${CMAKE_SOURCE_DIR}/src/include/pipe_layer.h
)

# #######FIND LIBRARIES#######
FIND_PACKAGE(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
	message(ERROR "OpenGL not found!")
endif(NOT OPENGL_FOUND)

message(STATUS "Found OpenGL Base")

# ##########GLFW BUILD###########
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/glfw)
include_directories(${CMAKE_SOURCE_DIR}/glfw/include)

#############PCG BUILD############
add_library(libpcg STATIC ${PCG_FILES})
include_directories(${CMAKE_SOURCE_DIR}/pcg)

set(LIBS ${OPENGL_LIBRARIES} glfw ${CMAKE_DL_LIBS} libpcg)
include_directories(${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})
LINK_LIBRARIES(${LIBS})

add_executable(gltesting ${CMAKE_SOURCE_DIR}/src/scratch/glTesting.cpp ${GLAD_FILES} ${GL_PIPES_SOURCE} ${HEADER_FILES})
target_link_libraries(gltesting ${LIBS})

add_executable(pipelayer_test ${CMAKE_SOURCE_DIR}/src/scratch/pipelayer_test.cpp ${GL_PIPES_SOURCE} ${HEADER_FILES} ${GLAD_FILES})
target_link_libraries(pipelayer_test ${LIBS})

add_executable(gl-pipes ${GL_PIPES_SOURCE} ${HEADER_FILES} ${GLAD_FILES})
TARGET_LINK_LIBRARIES(gl-pipes ${LIBS})
