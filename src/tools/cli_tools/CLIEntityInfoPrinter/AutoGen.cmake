include(${DEV_ROOT_PATH}/core/command_line/AutoGen.cmake)
include(	${DEV_ROOT_PATH}/tools/cli_tools/common/AutoGen.cmake)

# Define global headers here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/cli_tools/CLIEntityInfoPrinter/CLIEntityInfoPrinter.h
)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/cli_tools/CLIEntityInfoPrinter/CLIEntityInfoPrinter.cpp
	${DEV_ROOT_PATH}/tools/cli_tools/CLIEntityInfoPrinter/main.cpp
	
)
