IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /MainStateMachine)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5_utils.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/MD5SecurityChecker.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/FileManager.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/main.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/ConfigFileParser.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/SystemLoader.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState/SponsorsState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/GameLoopState/GameLoopState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState/FirstInformationState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/LoadingState/LoadingState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/ExitState/ExitState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/StatesFactory.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/ConfigState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainMenuState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/HistoryState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/PreGameState/PreGameState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/LoadingState/LoadingState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/VideoState/VideoState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState/SponsorsState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/ExitState/ExitState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState/FirstInformationState.cpp
	${DEV_ROOT_PATH}/MainStateMachine/MainTransitionFunction.cpp
	${DEV_ROOT_PATH}/MainStateMachine/MainStateMachine.cpp
	${DEV_ROOT_PATH}/MainStateMachine/MainTFBuilder.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5_utils.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/FileManager.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/MD5SecurityChecker.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/ConfigFileParser.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/SystemLoader.h
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState/SponsorsState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/GameLoopState/GameLoopState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState/FirstInformationState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/LoadingState/LoadingState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/ExitState/ExitState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/VideoRange.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/ConfigState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/StatesFactory.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/IState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainMenuState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/HistoryState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/CreditsState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/MainState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState/VideoRange.h
	${DEV_ROOT_PATH}/MainStateMachine/States/PreGameState/PreGameState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/LoadingState/LoadingState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/VideoState/VideoState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState/SponsorsState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/ExitState/ExitState.h
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState/FirstInformationState.h
	${DEV_ROOT_PATH}/MainStateMachine/MainTransitionFunction.h
	${DEV_ROOT_PATH}/MainStateMachine/IMainState.h
	${DEV_ROOT_PATH}/MainStateMachine/MainTFBuilder.h
	${DEV_ROOT_PATH}/MainStateMachine/MainStateMachineDefs.h
	${DEV_ROOT_PATH}/MainStateMachine/MainStateMachine.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/MainStateMachine
	${DEV_ROOT_PATH}/MainStateMachine/States
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState
	${DEV_ROOT_PATH}/MainStateMachine/States/ExitState
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState
	${DEV_ROOT_PATH}/MainStateMachine/States/VideoState
	${DEV_ROOT_PATH}/MainStateMachine/States/LoadingState
<<<<<<< HEAD
	${DEV_ROOT_PATH}/MainStateMachine/States/PreGameState
	${DEV_ROOT_PATH}/MainStateMachine/States/MainMenuState
#	${DEV_ROOT_PATH}/MainStateMachine/States/GameLoopState
=======
	${DEV_ROOT_PATH}/MainStateMachine/States/FirstInformationState
	${DEV_ROOT_PATH}/MainStateMachine/States/GameLoopState
	${DEV_ROOT_PATH}/MainStateMachine/States/SponsorsState
>>>>>>> origin/S3-GLS
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib
)

include_directories(${ACTUAL_DIRS})
