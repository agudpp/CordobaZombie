IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /LoadingSystem/Loaders/EnvironmentLoader)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/EnvironmentLoader/EnvironmentLoader.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/EnvironmentLoader/EnvironmentLoader.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/EnvironmentLoader
)

include_directories(${ACTUAL_DIRS})