
# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/engine/modules/IModuleLoader.h
    
    ${DEV_ROOT_PATH}/engine/modules/InputSystemLoader.h
    ${DEV_ROOT_PATH}/engine/modules/OgreSystemLoader.h
    ${DEV_ROOT_PATH}/engine/modules/ResourcesSystemLoader.h
    ${DEV_ROOT_PATH}/engine/modules/SoundSystemLoader.h
    ${DEV_ROOT_PATH}/engine/modules/VideoSystemLoader.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/engine/modules/InputSystemLoader.cpp
    ${DEV_ROOT_PATH}/engine/modules/OgreSystemLoader.cpp
    ${DEV_ROOT_PATH}/engine/modules/ResourcesSystemLoader.cpp
    ${DEV_ROOT_PATH}/engine/modules/SoundSystemLoader.cpp
    ${DEV_ROOT_PATH}/engine/modules/VideoSystemLoader.cpp
)


