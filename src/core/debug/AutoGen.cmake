# Define global headers here
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/core/debug/DebugUtil.h
	${DEV_ROOT_PATH}/core/debug/OgreNameGen.h	
	${DEV_ROOT_PATH}/core/debug/OgreText.h
	${DEV_ROOT_PATH}/core/debug/OgreTextTable.h
	${DEV_ROOT_PATH}/core/debug/PrimitiveDrawer.h
    ${DEV_ROOT_PATH}/core/debug/OgreCoreCreatorHelper.h	
)

# Define global sources here
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/core/debug/OgreNameGen.cpp
	${DEV_ROOT_PATH}/core/debug/OgreText.cpp
	${DEV_ROOT_PATH}/core/debug/PrimitiveDrawer.cpp
    ${DEV_ROOT_PATH}/core/debug/OgreCoreCreatorHelper.cpp	
)
