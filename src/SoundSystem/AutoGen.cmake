IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /SoundSystem)
set(SOUND_LIBRARIES openal vorbis vorbisfile)
set(OPENAL_LIBRARIES ${SOUND_LIBRARIES})

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/SoundSystem/SSS/SSoundSource.cpp
	${DEV_ROOT_PATH}/SoundSystem/LSS/LSoundSource.cpp
	${DEV_ROOT_PATH}/SoundSystem/SoundFamilyTable.cpp
	${DEV_ROOT_PATH}/SoundSystem/BufferBuilder.cpp
	${DEV_ROOT_PATH}/SoundSystem/SoundBuffer.cpp
	${DEV_ROOT_PATH}/SoundSystem/SoundManager.cpp
	${DEV_ROOT_PATH}/SoundSystem/SoundAPI.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/SoundSystem/SSS/SSoundSource.h
	${DEV_ROOT_PATH}/SoundSystem/LSS/LSoundSource.h
	${DEV_ROOT_PATH}/SoundSystem/SoundFamilyTable.h
	${DEV_ROOT_PATH}/SoundSystem/SoundEnums.h
	${DEV_ROOT_PATH}/SoundSystem/SoundBuffer.h
	${DEV_ROOT_PATH}/SoundSystem/SoundAPI.h
	${DEV_ROOT_PATH}/SoundSystem/SoundManager.h
	${DEV_ROOT_PATH}/SoundSystem/BufferBuilder.h
	${DEV_ROOT_PATH}/SoundSystem/SoundSource.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/SoundSystem
	${DEV_ROOT_PATH}/SoundSystem/LSS
	${DEV_ROOT_PATH}/SoundSystem/SSS
)

include_directories(${ACTUAL_DIRS})
