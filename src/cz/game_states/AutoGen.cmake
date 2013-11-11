# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/cz/game_states/IMainState.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateBuilder.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateEvent.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateID.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateInformation.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateMachine.h
	${DEV_ROOT_PATH}/cz/game_states/MainStateTT.h
	
	# states
	${DEV_ROOT_PATH}/cz/game_states/states/ExitMainState/ExitMainState.h
	${DEV_ROOT_PATH}/cz/game_states/states/InfoMainState/InfoMainState.h
	${DEV_ROOT_PATH}/cz/game_states/states/IntroMainState/IntroMainState.h
	${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MenuMainState.h
	${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/PlayingMainState.h
	${DEV_ROOT_PATH}/cz/game_states/states/PrePlayIntroMainState/PrePlayIntroMainState.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/cz/game_states/IMainState.cpp
	${DEV_ROOT_PATH}/cz/game_states/MainStateBuilder.cpp
	${DEV_ROOT_PATH}/cz/game_states/MainStateMachine.cpp
	${DEV_ROOT_PATH}/cz/game_states/MainStateTT.cpp
	
	# states
#	${DEV_ROOT_PATH}/cz/game_states/states/ExitMainState/ExitMainState.cpp
#	${DEV_ROOT_PATH}/cz/game_states/states/InfoMainState/InfoMainState.cpp
	${DEV_ROOT_PATH}/cz/game_states/states/IntroMainState/IntroMainState.cpp
#	${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MenuMainState.cpp
#	${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/PlayingMainState.cpp
#	${DEV_ROOT_PATH}/cz/game_states/states/PrePlayIntroMainState/PlayingMainState.cpp
)


