# Dependencies with our modules
include(${DEV_ROOT_PATH}/tools/qt_tools/qt_debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/resources_handler/AutoGen.cmake)
include(${DEV_ROOT_PATH}/io_interfaces/IO_InterfacesAll.cmake)


################################################################################
##                            Files to include                                ##
################################################################################

# Module headers
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/OgreWidget.h
    ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/QtOgreAppBase.h
)

# Module sources
set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/OgreWidget.cpp
    ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/QtOgreAppBase.cpp
)


################################################################################
##                             Qt configuration                               ##
################################################################################

# Find includes in corresponding build directories, e.g. for ui_generated.h
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Find required Qt library
IF (NOT DEFINED ENV{QTDIR})
	MESSAGE( FATAL_ERROR "Environment variable \"QTDIR\" must point "
                         "to the directory containing Qt4 include/ subdir, "
						 "e.g. /home/yourname/qt-everywhere-opensource-src-4.8")
ELSE()
	SET(QTDIR $ENV{QTDIR})
ENDIF(NOT DEFINED ENV{QTDIR})
IF (NOT EXISTS ${QTDIR}/include/QtCore/)
	MESSAGE( FATAL_ERROR "Environment variable \"QTDIR\" must point "
                         "to the directory containing Qt4 include/ subdir, "
						 "e.g. /home/yourname/qt-everywhere-opensource-src-4.8")
ENDIF(NOT EXISTS ${QTDIR}/include/QtCore/)

# Declare the Qt packages we need
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

# Include the corresponding Qt headers directories
INCLUDE(${CMAKE_ROOT}/Modules/FindQt4.cmake)
INCLUDE_DIRECTORIES(${QT_INCLUDES})
INCLUDE(${QT_USE_FILE})

# Use the compile definitions declared in the Qt libraries module
ADD_DEFINITIONS(-g ${QT_DEFINITIONS})

# Add compiler flags for building executables (-fPIE)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${QT_EXECUTABLE_COMPILE_FLAGS})

# Generate intermediate <moc>, <ui> and <resources> files for our build
SET(QtApp_MOCS ${QtApp_MOCS} 
               ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/OgreWidget.h
               ${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/QtOgreAppBase.h)
SET(QtApp_UIS ${QtApp_UIS})
SET(QtApp_RCCS ${QtApp_RCCS})

QT4_ADD_RESOURCES(RCCS_WRAPPER ${QtApp_RCCS})
QT4_WRAP_CPP(MOCS_WRAPPER ${QtApp_MOCS} )
QT4_WRAP_UI(UIS_WRAPPER ${QtApp_UIS} )

# Add the extra wrapper files
SET(HDRS ${HDRS} ${MOCS_WRAPPER} ${UIS_WRAPPER})
SET(SRCS ${SRCS} ${RCCS_WRAPPER})


################################################################################
##                           Libraries linkage                                ##
################################################################################

# Third party libraries paths
IF (NOT DEFINED THIRD_PARTY_LIBS)
	IF (NOT DEFINED ENV{CZ01_THIRD_PARTY})
		MESSAGE( FATAL_ERROR "Environment variable "
			"\"CZ01_THIRD_PARTY\" must be set for compilation.")
	ELSE()
		SET(THIRD_PARTY_LIBS $ENV{CZ01_THIRD_PARTY})
	ENDIF(NOT DEFINED ENV{CZ01_THIRD_PARTY})
ENDIF(NOT DEFINED THIRD_PARTY_LIBS)
INCLUDE_DIRECTORIES(
	${THIRD_PARTY_LIBS}/include
)


# Dynamic libraries
IF(WIN32)
	SET(COMMON_LIBRARIES ${COMMON_LIBRARIES}
	    ${QT_LIBRARIES}     # Qt
	)
ELSEIF(UNIX)
	SET(COMMON_LIBRARIES ${COMMON_LIBRARIES}
		${QT_LIBRARIES}  # Qt
	)
ENDIF()



