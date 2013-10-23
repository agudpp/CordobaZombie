IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
#include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /multimedia/video)


include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/multimedia/video/VideoPlayer.cpp
	${DEV_ROOT_PATH}/multimedia/video/OgreVideoPlayer.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/multimedia/video/VideoBuffer.h
	${DEV_ROOT_PATH}/multimedia/video/VideoPlayer.h
	${DEV_ROOT_PATH}/multimedia/video/OgreVideoPlayer.h
	
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/multimedia/video
	${DEV_ROOT_PATH}/multimedia/video/unittest
)


include_directories(${ACTUAL_DIRS})


# Specific libs
set(OPENAL_LIBRARIES openal vorbis vorbisfile)
set(COMMON_LIBRARIES 
	${COMMON_LIBRARIES}
	${OPENAL_LIBRARIES}
	avcodec avutil avformat swscale )


# Flag because UINT64_C macro is defined in standard C header stdint.h, 
# but only for non-cplusplus compiled code. That was giving problem at
# VideoPlayer.cpp while including avcodecs headers.
add_definitions(-D__STDC_CONSTANT_MACROS)
