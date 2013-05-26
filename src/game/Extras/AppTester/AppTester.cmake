
IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()

IF(NOT APPTESTER_PATH)
	set(APPTESTER_PATH .)
endif()


set(APPTESTER_SOURCES

${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.cpp
${APPTESTER_PATH}/AppTester.cpp
)

set(APPTESTER_HEADERS

${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.h
${APPTESTER_PATH}/AppTester.h
)


set(APPTESTER_DIRS
${APPTESTER_PATH}
${DEV_ROOT_PATH}/Common/GlobalObjects
)
