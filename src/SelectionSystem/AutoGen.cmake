IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /SelectionSystem)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/SelectionSystem/SelectionManager.cpp
	${DEV_ROOT_PATH}/SelectionSystem/SelectableObject.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/SelectionSystem/SelectableObject.h
	${DEV_ROOT_PATH}/SelectionSystem/SelectionType.h
	${DEV_ROOT_PATH}/SelectionSystem/SelectionManager.h
	${DEV_ROOT_PATH}/SelectionSystem/SelectionData.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/SelectionSystem
)

include_directories(${ACTUAL_DIRS})
