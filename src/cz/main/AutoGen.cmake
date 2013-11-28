include(${DEV_ROOT_PATH}/engine/AutoGen.cmake)

include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)

include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)

include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)

include(${DEV_ROOT_PATH}/effect/AutoGen.cmake)

include(${DEV_ROOT_PATH}/multimedia/video/AutoGen.cmake)
include(${DEV_ROOT_PATH}/multimedia/sound/AutoGen.cmake)

include(${DEV_ROOT_PATH}/ui/frontend/AutoGen.cmake)

include(${DEV_ROOT_PATH}/cz/game_states/AutoGen.cmake)
include(${DEV_ROOT_PATH}/cz/global_data/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/cz/main/GameRunner.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/cz/main/GameRunner.cpp
    ${DEV_ROOT_PATH}/cz/main/main.cpp
)


