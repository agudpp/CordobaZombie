include(${DEV_ROOT_PATH}/ui/overlay_effects/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/ui/frontend/FEInfo.h
    ${DEV_ROOT_PATH}/ui/frontend/FEElement.h
    ${DEV_ROOT_PATH}/ui/frontend/FEManager.h
    ${DEV_ROOT_PATH}/ui/frontend/FEOgreElement.h
    ${DEV_ROOT_PATH}/ui/frontend/element/button/FESimpleButton.h
    
    # effects
    ${DEV_ROOT_PATH}/ui/frontend/effects/FESimpleButtonEffect.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/ui/frontend/FEElement.cpp
    ${DEV_ROOT_PATH}/ui/frontend/FEManager.cpp
    ${DEV_ROOT_PATH}/ui/frontend/element/button/FESimpleButton.cpp
    
    # effects
    ${DEV_ROOT_PATH}/ui/frontend/effects/FESimpleButtonEffect.cpp
)

