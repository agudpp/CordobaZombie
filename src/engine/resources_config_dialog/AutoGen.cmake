
################################################################################
##                            Files to include                                ##
################################################################################

# Module headers
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.h
)

# Module sources
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.cpp
	${DEV_ROOT_PATH}/engine/resources_config_dialog/main.cpp
)


################################################################################
##                             Qt configuration                               ##
################################################################################

# Find includes in corresponding build directories, e.g. for ui_generated.h
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find required Qt libraries
if (NOT DEFINED ENV{QT_ROOT_PATH})
	message( FATAL_ERROR "Environment variable \"QT_ROOT_PATH\" must point "
                         "to the base directory of Qt5 library, e.g. "
                         "/home/yourname/Qt5.1.1")
else()
	set(QT_ROOT_PATH $ENV{QT_ROOT_PATH})
endif()
set(Qt5Widgets_DIR ${QT_ROOT_PATH}/lib/cmake/Qt5Widgets/)
find_package(Qt5Widgets)

# Add the include directories for the Qt 5 libraries to the compile lines
include_directories(${Qt5Widgets_INCLUDE_DIRS})
# Use the compile definitions defined in the Qt 5 libraries module
add_definitions(-g ${Qt5Widgets_DEFINITIONS})

# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
	${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}
)

# Generate intermediate moc and ui files for every header
QT5_WRAP_CPP(MOC_WRAPPERS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.h)
QT5_WRAP_UI(UI_WRAPPERS ${DEV_ROOT_PATH}/engine/resources_config_dialog/CbaZombieConfigDialog.ui)

# Add the extra wrapper files
set(HDRS ${HDRS} ${MOC_WRAPPERS} ${UI_WRAPPERS})


################################################################################
##                           Libraries linkage                                ##
################################################################################

# Third party libraries paths
if (NOT DEFINED THIRD_PARTY_LIBS)
	if (NOT DEFINED ENV{CZ01_THIRD_PARTY})
		message( FATAL_ERROR "Environment variable "
			"\"CZ01_THIRD_PARTY\" must be set for compilation.")
	else()
		set(THIRD_PARTY_LIBS $ENV{CZ01_THIRD_PARTY})
	endif()
endif()
include_directories(
	${THIRD_PARTY_LIBS}/include
	${THIRD_PARTY_LIBS}/lib
)

# Dynamic libraries
set(COMMON_LIBRARIES ${COMMON_LIBRARIES}
	${Qt5Widgets_LIBRARIES}  # Qt
#	tinyxml
)

# Static libraries
if(UNIX)
#	# tinyxml
#	add_library(tinyxml STATIC IMPORTED)
#	set_property(TARGET tinyxml PROPERTY
 #               IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/tinyxml.a)
endif(UNIX)

