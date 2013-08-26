# Define global sources here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointAStar.h
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointDefines.h
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointGraph.h
	${DEV_ROOT_PATH}/gps/pathfinding/WPPathfinder.h
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointGraphBuilder.h	
)

set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointAStar.cpp
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointGraph.cpp
	${DEV_ROOT_PATH}/gps/pathfinding/WPPathfinder.cpp
	${DEV_ROOT_PATH}/gps/pathfinding/WayPointGraphBuilder.cpp
)


