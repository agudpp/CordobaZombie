# check if this module is already included
if (NOT CRASH_HANDLER_DEFINED)
    set (CRASH_HANDLER_DEFINED TRUE)
    
    
    # Define global headers here
    set(HDRS ${HDRS}
        ${DEV_ROOT_PATH}/core/crash_handler/CrashHandler.h
    )
    
    # now we need to include files depending if we are on windows or linux
    # and set also some libs to be used by the compiler
    if(WIN32)
        # windows
        set(SRCS ${SRCS}
            ${DEV_ROOT_PATH}/core/crash_handler/CrashHandler_Win.cpp
        )
        
        # set the libraries
        set(COMMON_LIBRARIES {COMMON_LIBRARIES} bfd iberty imagehlp intl z)
        
    else()
        # linux
        set(SRCS ${SRCS}
            ${DEV_ROOT_PATH}/core/crash_handler/CrashHandler_Linux.cpp  
        )
        
    endif()

endif()
