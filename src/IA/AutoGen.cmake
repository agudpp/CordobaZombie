IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /IA)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/IA/FSM/SMTransitionTable.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/IA/FSM/SMTransitionTable.h
	${DEV_ROOT_PATH}/IA/FSM/StateMachine.h
	${DEV_ROOT_PATH}/IA/FSM/SMDefines.h
	${DEV_ROOT_PATH}/IA/FSM/IState.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/IA
	${DEV_ROOT_PATH}/IA/FSM
)

include_directories(${ACTUAL_DIRS})
