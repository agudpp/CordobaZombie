IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /VideoPlayer/FfmpegVideoPlayer)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/VideoPlayer/FfmpegVideoPlayer/FfmpegVideoPlayerAPI.cpp
	${DEV_ROOT_PATH}/VideoPlayer/FfmpegVideoPlayer/FfmpegVideoPlayer.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/VideoPlayer/FfmpegVideoPlayer/FfmpegVideoPlayer.h
	${DEV_ROOT_PATH}/VideoPlayer/FfmpegVideoPlayer/FfmpegVideoPlayerAPI.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/VideoPlayer/FfmpegVideoPlayer
)

include_directories(${ACTUAL_DIRS})
