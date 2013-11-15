include(${DEV_ROOT_PATH}/core/command_line/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/helpers/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/helpers/AutoGen.cmake)

include(${DEV_ROOT_PATH}/tools/cli_tools/common/AutoGen.cmake)

# Define global headers here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/tools/cli_tools/CLICollisionExporter/CLICollisionExporter.h
)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/tools/cli_tools/CLICollisionExporter/CLICollisionExporter.cpp
	${DEV_ROOT_PATH}/tools/cli_tools/CLICollisionExporter/main.cpp
	
)
