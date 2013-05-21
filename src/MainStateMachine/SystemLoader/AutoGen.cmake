IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /MainStateMachine/SystemLoader)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5_utils.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/FileManager.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/MD5SecurityChecker.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/main.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/SystemLoader.cpp
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/ConfigFileParser.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib/md5_utils.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/FileManager.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/MD5SecurityChecker.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/SystemLoader.h
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/ConfigFileParser.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker
	${DEV_ROOT_PATH}/MainStateMachine/SystemLoader/MD5SecurityChecker/md5Lib
)

include_directories(${ACTUAL_DIRS})
