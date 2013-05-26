IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Common/Math)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Common/Math/IntersectDetect.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Common/Math/Vec2.h
	${DEV_ROOT_PATH}/Common/Math/CommonMath.h
	${DEV_ROOT_PATH}/Common/Math/AABB.h
	${DEV_ROOT_PATH}/Common/Math/IntersectDetect.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Common/Math
)

include_directories(${ACTUAL_DIRS})
