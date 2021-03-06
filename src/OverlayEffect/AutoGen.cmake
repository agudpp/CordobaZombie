IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /OverlayEffect)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/OverlayEffect/Effects/Fade.cpp
	${DEV_ROOT_PATH}/OverlayEffect/Effects/Slide.cpp
	${DEV_ROOT_PATH}/OverlayEffect/MenuButtonEffect/MenuButtonEffect.cpp
	${DEV_ROOT_PATH}/OverlayEffect/OverlayEffect.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/OverlayEffect/Effects/Fade.h
	${DEV_ROOT_PATH}/OverlayEffect/Effects/Slide.h
	${DEV_ROOT_PATH}/OverlayEffect/MenuButtonEffect/MenuButtonEffect.h
	${DEV_ROOT_PATH}/OverlayEffect/OverlayEffectManager.h
	${DEV_ROOT_PATH}/OverlayEffect/EffectCb.h
	${DEV_ROOT_PATH}/OverlayEffect/OverlayEffect.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/OverlayEffect
	${DEV_ROOT_PATH}/OverlayEffect/MenuButtonEffect
	${DEV_ROOT_PATH}/OverlayEffect/Effects
)

include_directories(${ACTUAL_DIRS})
