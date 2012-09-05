IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /MicroAppRunner)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/MicroAppRunner/MicroApps/ImgShowerApp/ImgShowerApp.cpp
	${DEV_ROOT_PATH}/MicroAppRunner/MicroAppRunner.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/MicroAppRunner/MicroApps/ImgShowerApp/ImgShowerApp.h
	${DEV_ROOT_PATH}/MicroAppRunner/MicroAppRunner.h
	${DEV_ROOT_PATH}/MicroAppRunner/MicroApp.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/MicroAppRunner
	${DEV_ROOT_PATH}/MicroAppRunner/MicroApps
	${DEV_ROOT_PATH}/MicroAppRunner/MicroApps/ImgShowerApp
)

include_directories(${ACTUAL_DIRS})
