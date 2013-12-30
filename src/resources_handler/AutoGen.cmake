include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)


# ifnot def, define
if (NOT RESOURCE_HANDLER_DEFINED)

set (RESOURCE_HANDLER_DEFINED 1)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/resources_handler/ResourceGroup.h
	${DEV_ROOT_PATH}/resources_handler/ResourceHandler.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/resources_handler/ResourceHandler.cpp
)

endif()

