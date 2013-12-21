
################################################################################
##                            Files to include                                ##
################################################################################

# Module headers
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/engine/EngineConfiguration.h
	${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.h
)

# Module sources
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/engine/EngineConfiguration.cpp
	${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.cpp
	${DEV_ROOT_PATH}/engine/resources_config_dialog/main.cpp
)


################################################################################
##                             Qt configuration                               ##
################################################################################

# Find includes in corresponding build directories, e.g. for ui_generated.h
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Qt uses the QTDIR environment variable, so we need to change this QT_ROOT_PATH
# to QTDIR instead, now we will have to set both to the same dir.
#

# We will do this crap here because we need to release the fucking demo. This
# should be fixed later and we will use Qt4 everywhere...
#
if (WIN32)

# We need QtGui and QtCore for this
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

# include the directories of Qt
include_directories(${QT_INCLUDES})
include(${QT_USE_FILE})

# add the wrappers
# Generate intermediate <moc>, <ui> and <resources> files
SET(QtApp_MOCS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.h)
SET(QtApp_UIS  ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.ui)
SET(QtApp_RCCS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.qrc)
QT4_ADD_RESOURCES(RCCS_WRAPPER ${QtApp_RCCS})
QT4_WRAP_CPP(MOCS_WRAPPER ${QtApp_MOCS})
QT4_WRAP_UI(UIS_WRAPPER ${QtApp_UIS})

# Add the extra wrapper files
SET(HDRS ${HDRS} ${RCSS_WRAPPER} ${MOCS_WRAPPER} ${UIS_WRAPPER})

# Dynamic libraries
SET(COMMON_LIBRARIES ${COMMON_LIBRARIES}
    ${QT_LIBRARIES}     # Qt
    OpenAL32            # OpenAL for windows
)

else()

# Find required Qt libraries
IF (NOT DEFINED ENV{QT_ROOT_PATH})
	MESSAGE( FATAL_ERROR "Environment variable \"QT_ROOT_PATH\" must point "
                         "to the directory containing Qt5 include/ subdir, "
						 "e.g. /home/yourname/Qt5.1.1/5.1.1/gcc_64/")
ELSE()
	SET(QT_ROOT_PATH $ENV{QT_ROOT_PATH})
endIF(NOT DEFINED ENV{QT_ROOT_PATH})

IF (NOT EXISTS ${QT_ROOT_PATH}/include/QtWidgets/)
	MESSAGE( FATAL_ERROR "Environment variable \"QT_ROOT_PATH\" must point "
                         "to the directory containing Qt5 include/ subdir, "
						 "e.g. /home/yourname/Qt5.1.1/5.1.1/gcc_64/")
ENDIF(NOT EXISTS ${QT_ROOT_PATH}/include/QtWidgets/)
SET(Qt5Widgets_DIR ${QT_ROOT_PATH}/lib/cmake/Qt5Widgets/)
FIND_PACKAGE(Qt5Widgets)

# Add the include directories for the Qt 5 libraries to the compile lines
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
# Use the compile definitions defined in the Qt 5 libraries module
ADD_DEFINITIONS(-g ${Qt5Widgets_DEFINITIONS})

# Add compiler flags for building executables (-fPIE)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
	${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}
)

# Generate intermediate <moc>, <ui> and <resources> files
SET(QtApp_MOCS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.h)
SET(QtApp_UIS  ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.ui)
SET(QtApp_RCCS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.qrc)
QT5_ADD_RESOURCES(RCCS_WRAPPER ${QtApp_RCCS})
QT5_WRAP_CPP(MOCS_WRAPPER ${QtApp_MOCS})
QT5_WRAP_UI(UIS_WRAPPER ${QtApp_UIS})

# Add the extra wrapper files
SET(HDRS ${HDRS} ${MOCS_WRAPPER} ${UIS_WRAPPER} ${RCSS_WRAPPER})


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
	${THIRD_PARTY_LIBS}/lib
)

# Dynamic libraries
SET(COMMON_LIBRARIES ${COMMON_LIBRARIES}
	${Qt5Widgets_LIBRARIES}  # Qt
	openal
)

# Static libraries
IF(UNIX)
#	# tinyxml
#	add_library(tinyxml STATIC IMPORTED)
#	set_property(TARGET tinyxml PROPERTY
 #               IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/tinyxml.a)
ENDIF(UNIX)

endif()

