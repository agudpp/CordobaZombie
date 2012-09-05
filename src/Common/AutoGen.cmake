IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /Common)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/Common/QueueContainer/QueueContainerObject.cpp
	${DEV_ROOT_PATH}/Common/DotSceneLoader/DotSceneLoader.cpp
	${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.cpp
	${DEV_ROOT_PATH}/Common/DebugUtil/PrimitiveDrawer.cpp
	${DEV_ROOT_PATH}/Common/Util/Util.cpp
	${DEV_ROOT_PATH}/Common/Math/IntersectDetect.cpp
	${DEV_ROOT_PATH}/Common/GUIHelper/GUIHelper.cpp
	${DEV_ROOT_PATH}/Common/XMLHelper/XMLHelper.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/Common/QueueContainer/QueueContainer.h
	${DEV_ROOT_PATH}/Common/QueueContainer/QueueContainerObject.h
	${DEV_ROOT_PATH}/Common/DotSceneLoader/DotSceneLoader.h
	${DEV_ROOT_PATH}/Common/GlobalObjects/GlobalObjects.h
	${DEV_ROOT_PATH}/Common/DebugUtil/PrimitiveDrawer.h
	${DEV_ROOT_PATH}/Common/DebugUtil/DebugUtil.h
	${DEV_ROOT_PATH}/Common/Util/Util.h
	${DEV_ROOT_PATH}/Common/Math/Vec2.h
	${DEV_ROOT_PATH}/Common/Math/CommonMath.h
	${DEV_ROOT_PATH}/Common/Math/IntersectDetect2.h
	${DEV_ROOT_PATH}/Common/Math/AABB.h
	${DEV_ROOT_PATH}/Common/Math/IntersectDetect.h
	${DEV_ROOT_PATH}/Common/GUIHelper/GUIHelper.h
	${DEV_ROOT_PATH}/Common/XMLHelper/XMLHelper.h
	${DEV_ROOT_PATH}/Common/tinyxml/lex_util.h
	${DEV_ROOT_PATH}/Common/tinyxml/lex_token.h
	${DEV_ROOT_PATH}/Common/tinyxml/tinyxml.h
	${DEV_ROOT_PATH}/Common/tinyxml/action_store.h
	${DEV_ROOT_PATH}/Common/tinyxml/htmlutil.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_stack.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_expression.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_stream.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_processor.h
	${DEV_ROOT_PATH}/Common/tinyxml/byte_stream.h
	${DEV_ROOT_PATH}/Common/tinyxml/tinystr.h
	${DEV_ROOT_PATH}/Common/tinyxml/node_set.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_static.h
	${DEV_ROOT_PATH}/Common/tinyxml/tinyxpath_conf.h
	${DEV_ROOT_PATH}/Common/tinyxml/tokenlist.h
	${DEV_ROOT_PATH}/Common/tinyxml/xpath_syntax.h
	${DEV_ROOT_PATH}/Common/tinyxml/xml_util.h
	${DEV_ROOT_PATH}/Common/MultiplatformTypedefs.h
	${DEV_ROOT_PATH}/Common/GeneralTypedefs.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/Common
	${DEV_ROOT_PATH}/Common/tinyxml
	${DEV_ROOT_PATH}/Common/XMLHelper
	${DEV_ROOT_PATH}/Common/GUIHelper
	${DEV_ROOT_PATH}/Common/Math
	${DEV_ROOT_PATH}/Common/Util
	${DEV_ROOT_PATH}/Common/DebugUtil
	${DEV_ROOT_PATH}/Common/GlobalObjects
	${DEV_ROOT_PATH}/Common/DotSceneLoader
	${DEV_ROOT_PATH}/Common/QueueContainer
)

include_directories(${ACTUAL_DIRS})
