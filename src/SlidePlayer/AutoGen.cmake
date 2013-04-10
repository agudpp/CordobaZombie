IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /SlidePlayer)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/SlidePlayer/SlidePlayer.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/SlidePlayer/SlidePlayer.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/SlidePlayer
)

include_directories(${ACTUAL_DIRS})
