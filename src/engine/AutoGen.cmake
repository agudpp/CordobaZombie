include(${DEV_ROOT_PATH}/engine/modules/AutoGen.cmake)


# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/engine/Engine.h
    ${DEV_ROOT_PATH}/engine/EngineLoader.h
    ${DEV_ROOT_PATH}/engine/EngineConfiguration.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/engine/Engine.cpp
    ${DEV_ROOT_PATH}/engine/EngineLoader.cpp
    ${DEV_ROOT_PATH}/engine/EngineConfiguration.cpp
)


