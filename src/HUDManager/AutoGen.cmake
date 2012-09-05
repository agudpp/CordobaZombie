IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /HUDManager)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayerStatusBuilder.cpp
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayersStatusPanel.cpp
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayerStatus.cpp
	${DEV_ROOT_PATH}/HUDManager/Cellphone/Cellphone.cpp
	${DEV_ROOT_PATH}/HUDManager/Cellphone/CellphoneMap.cpp
	${DEV_ROOT_PATH}/HUDManager/ExtraActionPanel/ExtraActionPanel.cpp
	${DEV_ROOT_PATH}/HUDManager/ExtraActionPanel/ExtraAction.cpp
	${DEV_ROOT_PATH}/HUDManager/PassportPicture/PassportPicture.cpp
	${DEV_ROOT_PATH}/HUDManager/HudElement.cpp
	${DEV_ROOT_PATH}/HUDManager/HUDManager.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayerStatus.h
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayersStatusPanel.h
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel/PlayerStatusBuilder.h
	${DEV_ROOT_PATH}/HUDManager/Cellphone/CellphoneMapInfo.h
	${DEV_ROOT_PATH}/HUDManager/Cellphone/CellphoneMap.h
	${DEV_ROOT_PATH}/HUDManager/Cellphone/CellphoneElement.h
	${DEV_ROOT_PATH}/HUDManager/Cellphone/Cellphone.h
	${DEV_ROOT_PATH}/HUDManager/ExtraActionPanel/ExtraAction.h
	${DEV_ROOT_PATH}/HUDManager/ExtraActionPanel/ExtraActionPanel.h
	${DEV_ROOT_PATH}/HUDManager/PassportPicture/PassportPicture.h
	${DEV_ROOT_PATH}/HUDManager/HUDManager.h
	${DEV_ROOT_PATH}/HUDManager/HudElement.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/HUDManager
	${DEV_ROOT_PATH}/HUDManager/PassportPicture
	${DEV_ROOT_PATH}/HUDManager/ExtraActionPanel
	${DEV_ROOT_PATH}/HUDManager/Backpack
	${DEV_ROOT_PATH}/HUDManager/Cellphone
	${DEV_ROOT_PATH}/HUDManager/PlayersStatusPanel
)

include_directories(${ACTUAL_DIRS})
