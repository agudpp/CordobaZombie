IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Extras/Helpers)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Extras/Helpers/MovableText.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Extras/Helpers/MovableText.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Extras/Helpers
)

include_directories(${ACTUAL_DIRS})
