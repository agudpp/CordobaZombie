
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
set(Qt5Widgets_DIR ${QT_ROOT_PATH}/5.1.1/gcc_64/lib/cmake/Qt5Widgets/)
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
QT5_WRAP_CPP(MOC_WRAPPERS CbaZombieConfigDialog.h)
QT5_WRAP_UI(UI_WRAPPERS CbaZombieConfigDialog.ui)

# Add the extra wrapper files
set(HDRS ${HDRS} ${MOC_WRAPPERS} ${UI_WRAPPERS})


################################################################################
##                           Libraries linkage                                ##
################################################################################

# Third party libraries paths
set(THIRD_PARTY_LIBS $ENV{CZ01_THIRD_PARTY})
include_directories(
	${THIRD_PARTY_LIBS}/include
	${THIRD_PARTY_LIBS}/lib
)

# Dynamic libraries
set(COMMON_LIBRARIES ${COMMON_LIBRARIES}
	tinyxml
	${Qt5Widgets_LIBRARIES}  # Qt
)

# Static libraries
if(UNIX)
	# tinyxml
	add_library(tinyxml STATIC IMPORTED)
	set_property(TARGET tinyxml PROPERTY
                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/tinyxml.a)
endif(UNIX)


################################################################################
##                            Files to include                                ##
################################################################################

# Module headers
set(HDRS ${HDRS}
	${ROOT_PATH}/src/BaseApplication.h
	${ROOT_PATH}/src/TutorialApplication.h
	${ROOT_PATH}/src/CbaZombieConfigDialog.h
	${ROOT_PATH}/src/GLX_backdrop.h
	${ROOT_PATH}/src/addressbook.h
)

# Module sources
set(SRCS ${SRCS}
	${ROOT_PATH}/src/BaseApplication.cpp
	${ROOT_PATH}/src/TutorialApplication.cpp
	${ROOT_PATH}/src/CbaZombieConfigDialog.cpp
	${ROOT_PATH}/src/addressbook.cpp
)

