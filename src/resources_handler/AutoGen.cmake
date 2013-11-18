include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)


# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/resources_handler/ResourceGroup.h
	${DEV_ROOT_PATH}/resources_handler/ResourceHandler.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/resources_handler/ResourceHandler.cpp
)


