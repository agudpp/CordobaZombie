
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

# Include third party libraries
include_directories(${THIRD_PARTY_LIBS}/lib)

# Dynamic libraries
set(COMMON_LIBRARIES ${COMMON_LIBRARIES} openal vorbisfile vorbisenc)

# Static libraries
if(UNIX)
	# ogg
	add_library(ogg STATIC IMPORTED)
	set_property(TARGET ogg PROPERTY  # point imported target to real file
	             IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libogg.a)
	# vorbis (core)
	add_library(vorbis STATIC IMPORTED)
	set_property(TARGET vorbis PROPERTY  # point imported target to real file
	             IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libvorbis.a)
endif(UNIX)
# TODO here we need to put Windows(TM) specific stuff

