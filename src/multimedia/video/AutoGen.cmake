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

## Specific libs

if (UNIX)
    set(OPENAL_LIBRARIES openal vorbis vorbisfile)
endif(UNIX)
if (WIN32)
    set(OPENAL_LIBRARIES OpenAL32 vorbisfile vorbisenc)
endif(WIN32)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} ${OPENAL_LIBRARIES}
    avcodec avutil avformat swscale)


# Static imported libs:

# avcodec static linkage 
#add_library(mavcodec STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET mavcodec PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libavcodec.a)

# avformat static linkage 
#add_library(mavformat STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET mavformat PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libavformat.a)

# swscale static linkage 
#add_library(mswscale STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET mswscale PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libswscale.a)

## avutil static linkage 
#add_library(mavutil STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET mavutil PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libavutil.a)

#set(COMMON_LIBRARIES ${COMMON_LIBRARIES} mavcodec mavformat mswscale mavutil)


# Flag because UINT64_C macro is defined in standard C header stdint.h, 
# but only for non-cplusplus compiled code. That was giving problem at
# VideoPlayer.cpp while including avcodecs headers.
add_definitions(-D__STDC_CONSTANT_MACROS)
