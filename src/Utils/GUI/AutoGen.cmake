IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Utils/GUI)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Utils/GUI/LoadingBar/LoadingBar.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Utils/GUI/AtlasOverlay/SingleAtlasOverlay.h
	${DEV_ROOT_PATH}/Utils/GUI/AtlasOverlay/MultiAtlasOverlay.h
	${DEV_ROOT_PATH}/Utils/GUI/LoadingBar/LoadingBar.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Utils/GUI
	${DEV_ROOT_PATH}/Utils/GUI/LoadingBar
	${DEV_ROOT_PATH}/Utils/GUI/AtlasOverlay
)

include_directories(${ACTUAL_DIRS})
