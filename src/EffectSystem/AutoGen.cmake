IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /EffectSystem)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/EffectSystem/EffectsBuilder.cpp
	${DEV_ROOT_PATH}/EffectSystem/Effect.cpp
	${DEV_ROOT_PATH}/EffectSystem/FakeEffect.cpp
	${DEV_ROOT_PATH}/EffectSystem/EffectsManager.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/EffectSystem/FakeEffect.h
	${DEV_ROOT_PATH}/EffectSystem/EffectsBuilder.h
	${DEV_ROOT_PATH}/EffectSystem/EffectsManager.h
	${DEV_ROOT_PATH}/EffectSystem/Effect.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/EffectSystem
	${DEV_ROOT_PATH}/EffectSystem/scripts
	${DEV_ROOT_PATH}/EffectSystem/Effects
	${DEV_ROOT_PATH}/EffectSystem/Effects/Blood
	${DEV_ROOT_PATH}/EffectSystem/Effects/Explosion
)

include_directories(${ACTUAL_DIRS})
