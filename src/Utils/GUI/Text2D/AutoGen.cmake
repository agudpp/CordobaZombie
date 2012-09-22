IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Utils/GUI/Text2D)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Utils/GUI/Text2D/Text2D.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Utils/GUI/Text2D/Text2D.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Utils/GUI/Text2D
)

include_directories(${ACTUAL_DIRS})
