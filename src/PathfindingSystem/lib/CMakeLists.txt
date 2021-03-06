#/*
#-----------------------------------------------------------------------------
#Filename:    CMakeLists.txt
#-----------------------------------------------------------------------------
#
#This source file is part of the
#   ___                 __    __ _ _    _ 
#  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
# //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
#/ \_// (_| | | |  __/  \  /\  /| |   <| |
#\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
#      |___/                              
#      Tutorial Framework
#      http://www.ogre3d.org/tikiwiki/
#-----------------------------------------------------------------------------
#*/
cmake_minimum_required(VERSION 2.6)
 
project(OgreApp)
 
if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
endif(WIN32)
 
if(UNIX)
	#set(CMAKE_BUILD_TYPE "debug")
	add_definitions(-DDEBUG)
	set(CMAKE_MODULE_PATH 
		"/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}"
		"/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}"	
	)
	set(OGRE_SAMPLES_INCLUDEPATH
		/usr/share/OGRE/samples/Common/include/
		/usr/local/share/OGRE/samples/Common/include/		
	)
endif(UNIX)
 
if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
 
set(CMAKE_DEBUG_POSTFIX "_d")
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 
find_package(OGRE REQUIRED)
 
#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#  message(SEND_ERROR "You need Ogre 1.8 Cthugha to build this.")
#endif()
 
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()
 
# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	if (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	find_package(Boost QUIET)
 
	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()
set (DEV_ROOT_PATH $ENV{ESTRATEGIA2012_DEV_PATH})

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${DEV_ROOT_PATH}/Common/DebugUtil
	${DEV_ROOT_PATH}/Common
	${DEV_ROOT_PATH}/Common/tinyxml
	${DEV_ROOT_PATH}/Common/GlobalObjects
	${DEV_ROOT_PATH}/Utils/MouseCursor
	
	${DEV_ROOT_PATH}/Extras/AppTester
)


set(CP /PathfindingSystem)

set(SRCS
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GEdge.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/Graph.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GNode.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/Triangle.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleFunnel.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleCache.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriNavMeshBuilder.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleAStar.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleNavMesh.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/PathfindingManager.cpp
)

set(HDRS
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GNode.h
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GEdge.h
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/Graph.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/ConvexPolyStructs.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/Triangle.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/PolyStructsContainer.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/ConvexPolygon.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/PolygonStructs.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleFunnel.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleAStar.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriNavMeshBuilder.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleCache.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleNavMesh.h
	${DEV_ROOT_PATH}/PathfindingSystem/PathfindingManager.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/PathfindingSystem
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh
	${DEV_ROOT_PATH}/PathfindingSystem/lib
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph
)

include_directories(${ACTUAL_DIRS})

include(${DEV_ROOT_PATH}/Common/AutoGen.cmake)
include(${DEV_ROOT_PATH}/Common/QueueContainer/AutoGen.cmake)

# Include libraries dependencies
include(${DEV_ROOT_PATH}/CollisionSystem/AutoGenLib.cmake)

SET(LIBRARY_OUTPUT_PATH 	./)

ADD_LIBRARY( PathfindingSystem STATIC
	${HDRS}
	${SRCS}
)

TARGET_LINK_LIBRARIES( PathfindingSystem CollisionSystem)





include(${DEV_ROOT_PATH}/Common/Math/AutoGen.cmake)
include(${DEV_ROOT_PATH}/Common/Util/AutoGen.cmake)
 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

