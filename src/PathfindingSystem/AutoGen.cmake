IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /PathfindingSystem)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GEdge.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/Graph.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GNode.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriNavMeshBuilder.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleFunnel.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleNavMesh.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleCache.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleAStar.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/Triangle.cpp
	${DEV_ROOT_PATH}/PathfindingSystem/PathfindingManager.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/Graph.h
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GNode.h
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph/GEdge.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleNavMesh.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleFunnel.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleAStar.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriangleCache.h
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh/TriNavMeshBuilder.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/PolyStructsContainer.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/PolygonStructs.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/ConvexPolyStructs.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/ConvexPolygon.h
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon/Triangle.h
	${DEV_ROOT_PATH}/PathfindingSystem/PathfindingManager.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/PathfindingSystem
	${DEV_ROOT_PATH}/PathfindingSystem/ConvexPolygon
	${DEV_ROOT_PATH}/PathfindingSystem/NavMesh
	${DEV_ROOT_PATH}/PathfindingSystem/AbstractGraph
)

include_directories(${ACTUAL_DIRS})
