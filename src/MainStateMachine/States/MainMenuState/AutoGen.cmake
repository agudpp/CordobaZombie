IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /MainStateMachine/States/MainMenuState)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/StatesFactory.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/ConfigState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainMenuState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/HistoryState.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/VideoRange.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/ConfigState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/HistoryState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/StatesFactory.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainMenuState.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState
)

include_directories(${ACTUAL_DIRS})
