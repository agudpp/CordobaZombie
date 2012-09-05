IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /MenuSystem)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/MenuSystem/GUI/StaticContainer/StaticContainer.cpp
	${DEV_ROOT_PATH}/MenuSystem/GUI/Button/MenuButton.cpp
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIObject.cpp
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIManager.cpp
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIBuilder.cpp
	${DEV_ROOT_PATH}/MenuSystem/MenuManager.cpp
	${DEV_ROOT_PATH}/MenuSystem/IMenu.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/MenuSystem/GUI/StaticContainer/StaticContainer.h
	${DEV_ROOT_PATH}/MenuSystem/GUI/Button/MenuButton.h
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIBuilder.h
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIManager.h
	${DEV_ROOT_PATH}/MenuSystem/GUI/GUIObject.h
	${DEV_ROOT_PATH}/MenuSystem/MenuDefs.h
	${DEV_ROOT_PATH}/MenuSystem/MenuManager.h
	${DEV_ROOT_PATH}/MenuSystem/IMenu.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/MenuSystem
	${DEV_ROOT_PATH}/MenuSystem/GUI
	${DEV_ROOT_PATH}/MenuSystem/GUI/Button
	${DEV_ROOT_PATH}/MenuSystem/GUI/StaticContainer
)

include_directories(${ACTUAL_DIRS})
