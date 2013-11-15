include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)

# Define global headers here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/tools/cli_tools/common/EntityInformation.h
    ${DEV_ROOT_PATH}/tools/cli_tools/common/XMLResourceParser.h
)

# Define global sources here
set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/tools/cli_tools/common/EntityInformation.cpp	
    ${DEV_ROOT_PATH}/tools/cli_tools/common/XMLResourceParser.cpp	
)
