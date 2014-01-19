include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/random_generator/RandomGenerator.h
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/random_generator/RandomGenerator.cpp
)

## for all OS, include following dynamic link libraries:
set(COMMON_LIBRARIES ${COMMON_LIBRARIES} m)

