
# check if this module was already defined
if (NOT OPENALHANDLER_DEFINED)
    set(OPENALHANDLER_DEFINED TRUE)
    
    # set the headers and sources
    set(HDRS ${HDRS}
        ${DEV_ROOT_PATH}/multimedia/openal_handler/OpenALHandler.h
    )
    set(SRCS ${SRCS}
        ${DEV_ROOT_PATH}/multimedia/openal_handler/OpenALHandler.cpp
    )
    
    # Dynamic libraries
    if (UNIX)
        set(COMMON_LIBRARIES ${COMMON_LIBRARIES} openal ogg vorbisfile vorbisenc)
    endif(UNIX)
    if (WIN32)
        set(COMMON_LIBRARIES ${COMMON_LIBRARIES} OpenAL32 ogg vorbisfile vorbisenc)
    endif(WIN32)
endif()


