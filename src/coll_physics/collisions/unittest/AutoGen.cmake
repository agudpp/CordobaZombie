include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}

)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/coll_physics/collisions/unittest/test.cpp
)

set(COMMON_LIBRARIES ${COMMON_LIBRARIES} UnitTest++)
