
# check if this module was already set
if (NOT VIDEOSYSTEM_DEFINED)

    set (VIDEOSYSTEM_DEFINED TRUE)
    
    # Flag because UINT64_C macro is defined in standard C header stdint.h, 
    # but only for non-cplusplus compiled code. That was giving problem at
    # VideoPlayer.cpp while including avcodecs headers.
    
    add_definitions(-D__STDC_CONSTANT_MACROS)


    include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
    include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
    include(${DEV_ROOT_PATH}/multimedia/openal_handler/AutoGen.cmake)
    
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

    # openal libs
    include(${DEV_ROOT_PATH}/multimedia/openal_handler/AutoGen.cmake)

    ## Specific libs
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES} avcodec avutil avformat swscale)

endif()
