
# check if this module was already included
if (NOT SOUNDSYSTEM_DEFINED)
    
	set (SOUNDSYSTEM_DEFINED TRUE)

    # Module headers
    set(HDRS ${HDRS}
    	${DEV_ROOT_PATH}/multimedia/sound/BufferBuilder.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundAPI.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundBuffer.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundEnums.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundFamilyTable.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundHandler.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundManager.h
    	${DEV_ROOT_PATH}/multimedia/sound/SoundSource.h
    	${DEV_ROOT_PATH}/multimedia/sound/LSS/LSoundSource.h
    	${DEV_ROOT_PATH}/multimedia/sound/SSS/SSoundSource.h
    )
    
    # Module sources
    set(SRCS ${SRCS}
    	${DEV_ROOT_PATH}/multimedia/sound/BufferBuilder.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/SoundBuffer.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/SoundFamilyTable.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/SoundHandler.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/SoundManager.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/LSS/LSoundSource.cpp
    	${DEV_ROOT_PATH}/multimedia/sound/SSS/SSoundSource.cpp
    )
    
    # include the openal and RNG libs
    include(${DEV_ROOT_PATH}/multimedia/openal_handler/AutoGen.cmake)
	include(${DEV_ROOT_PATH}/tools/random_generator/AutoGen.cmake)

endif()
