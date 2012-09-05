IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /LevelManager)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/LevelManager/LineOfSightManager/LineOfSightManager.cpp
	${DEV_ROOT_PATH}/LevelManager/RaycastManager/RaycastManager.cpp
	${DEV_ROOT_PATH}/LevelManager/LevelManager.cpp
	${DEV_ROOT_PATH}/Common/Util/Util.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/LevelManager/LineOfSightManager/LineOfSightManager.h
	${DEV_ROOT_PATH}/LevelManager/RaycastManager/RaycastManager.h
	${DEV_ROOT_PATH}/LevelManager/LevelManager.h
	${DEV_ROOT_PATH}/Common/Util/Util.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/LevelManager
	${DEV_ROOT_PATH}/LevelManager/RaycastManager
	${DEV_ROOT_PATH}/LevelManager/LineOfSightManager
	${DEV_ROOT_PATH}/Common/Util
	${DEV_ROOT_PATH}/Common/tinyxml
)

include_directories(${ACTUAL_DIRS})

