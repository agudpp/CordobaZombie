IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()

#include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/multimedia/video/AutoGen.cmake)


set(CP /tools/tests/video/ogre_independent)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/tools/tests/video/ogre_independent/OgreIndepVideoTest.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/tools/tests/video/ogre_independent/OgreIndepVideoTest.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/core/debug
	${DEV_ROOT_PATH}/tools/tests/video/ogre_independent
)

include_directories(${ACTUAL_DIRS})
