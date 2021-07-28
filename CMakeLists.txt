cmake_minimum_required(VERSION 2.8.3)
project(xsens_mti_driver LANGUAGES CXX)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
include_directories(
  lib/xspublic
)

set(MOOS_DIR "/opt/moos-ivp/build/MOOS/MOOSCore")
find_package(MOOS REQUIRED)
link_directories(${MOOS_LIBRARY_DIRS})
include_directories(${MOOS_INCLUDE_DIRS})
link_directories(${MOOS_LIBRARY_DIRS})

link_directories(lib/xspublic/xscontroller lib/xspublic/xscommon lib/xspublic/xstypes)
add_executable(
	xsens_mti_node
	src/main.cpp
	src/xdainterface.cpp
	src/xdacallback.cpp
	src/xdaprocess.cpp
)
target_link_libraries(
	xsens_mti_node
	xscontroller
	xscommon
	xstypes
	pthread
	dl
	${MOOS_LIBRARIES}
)

add_custom_command(TARGET xsens_mti_node
    PRE_BUILD COMMAND +make -C ${CMAKE_CURRENT_SOURCE_DIR}/lib/xspublic
)

add_custom_command(
	TARGET xsens_mti_node
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_SOURCE_DIR}/launch/xsens.moos
	${CMAKE_CURRENT_BINARY_DIR}/xsens.moos
)