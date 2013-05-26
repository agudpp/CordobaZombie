IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /LoadingSystem)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/CameraLimitsLoader/CameraLimitsLoader.cpp
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelPathfindLoader/LevelPathfindLoader.cpp
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelGeometryLoader/LevelGeometryLoader.cpp
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/ZombieLoader/ZombieLoader.cpp
	${DEV_ROOT_PATH}/LoadingSystem/LoaderManager.cpp
	${DEV_ROOT_PATH}/LoadingSystem/Loader.cpp
	${DEV_ROOT_PATH}/LoadingSystem/LoaderBuilder.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/CameraLimitsLoader/CameraLimitsLoader.h
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelPathfindLoader/LevelPathfindLoader.h
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelGeometryLoader/LevelGeometryLoader.h
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/ZombieLoader/ZombieLoader.h
	${DEV_ROOT_PATH}/LoadingSystem/LoaderData.h
	${DEV_ROOT_PATH}/LoadingSystem/LoaderManager.h
	${DEV_ROOT_PATH}/LoadingSystem/Loader.h
	${DEV_ROOT_PATH}/LoadingSystem/LoaderBuilder.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/LoadingSystem
	${DEV_ROOT_PATH}/LoadingSystem/Loaders
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/ZombieLoader
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelGeometryLoader
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/LevelLoaders/LevelPathfindLoader
	${DEV_ROOT_PATH}/LoadingSystem/Loaders/CameraLimitsLoader
)

include_directories(${ACTUAL_DIRS})
