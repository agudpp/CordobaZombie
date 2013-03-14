IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /InputManager)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/InputManager/RaycastStates/EmptySelection.cpp
	${DEV_ROOT_PATH}/InputManager/RaycastStates/MultiPlayerSel.cpp
	${DEV_ROOT_PATH}/InputManager/RaycastStates/ObjectSel.cpp
	${DEV_ROOT_PATH}/InputManager/RaycastStates/SinglePlayerSel.cpp
	${DEV_ROOT_PATH}/InputManager/InputManager.cpp
	${DEV_ROOT_PATH}/InputManager/InputStateMachine.cpp
	${DEV_ROOT_PATH}/InputManager/InputTransitionTable.cpp
	${DEV_ROOT_PATH}/InputManager/IInputState.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/InputManager/RaycastStates/EmptySelection.h
	${DEV_ROOT_PATH}/InputManager/RaycastStates/MultiPlayerSel.h
	${DEV_ROOT_PATH}/InputManager/RaycastStates/ObjectSel.h
	${DEV_ROOT_PATH}/InputManager/RaycastStates/SinglePlayerSel.h
	${DEV_ROOT_PATH}/InputManager/InputManager.h
	${DEV_ROOT_PATH}/InputManager/InputStateMachine.h
	${DEV_ROOT_PATH}/InputManager/IInputState.h
	${DEV_ROOT_PATH}/InputManager/InputMouse.h
	${DEV_ROOT_PATH}/InputManager/InputTransitionTable.h
	${DEV_ROOT_PATH}/InputManager/InputKeyboard.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/InputManager
	${DEV_ROOT_PATH}/InputManager/RaycastStates
)

include_directories(${ACTUAL_DIRS})
