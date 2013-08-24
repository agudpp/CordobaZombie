# Define global headers here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/trigger_system/TriggerAgent.h
	${DEV_ROOT_PATH}/core/trigger_system/TriggerSystem.h
	${DEV_ROOT_PATH}/core/trigger_system/TriggerSystemDefines.h
	${DEV_ROOT_PATH}/core/trigger_system/TriggerZone.h
#	${DEV_ROOT_PATH}/core/trigger_system/GraphColoringHelper.h
	${DEV_ROOT_PATH}/core/trigger_system/TriggerMatrix.h
)

# Define global sources here
set(SRCS ${SRCS}
#	${DEV_ROOT_PATH}/core/trigger_system/GraphColoringHelper.cpp
	${DEV_ROOT_PATH}/core/trigger_system/TriggerMatrix.cpp
	${DEV_ROOT_PATH}/core/trigger_system/TriggerAgent.cpp
	${DEV_ROOT_PATH}/core/trigger_system/TriggerSystem.cpp
)
