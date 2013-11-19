include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/app_tester/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/main_player/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/linkers/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/hud/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/tests/hud/HUDTest.h
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/tests/hud/HUDTest.cpp
	${DEV_ROOT_PATH}/tools/tests/hud/main.cpp
	${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
)

################################################################################
##                       SoundSystem specific stuff                          ###
################################################################################

if(UNIX)
#add_library(LinearMath STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET LinearMath PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libLinearMath.a)
endif(UNIX)
# TODO here we need to put Windows(TM) specific stuff

## for all OS, include following dynamic link libraries:
# Won't include the following: they're already included in the module's autogen.
#set(COMMON_LIBRARIES ${COMMON_LIBRARIES} openal ogg vorbis)
