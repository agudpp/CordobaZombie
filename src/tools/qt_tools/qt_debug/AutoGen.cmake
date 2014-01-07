

if (NOT QTDEBUG_DEFINED)

set(QTDEBUG_DEFINED 1)

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/tools/qt_tools/qt_debug/QtDebug.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/tools/qt_tools/qt_debug/QtDebug.cpp
)

endif()

