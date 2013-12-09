cmake_minimum_required(VERSION 2.8.3)

project(address_book)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find required Qt libraries
if (NOT DEFINED ENV{QT_ROOT_PATH})
	message( FATAL_ERROR "Environment variable \"QT_ROOT_PATH\" must point "
                         "to the install directory of Qt5 library, e.g. "
                         "/home/yourname/Qt5.1.1")
else()
	set(QT_ROOT_PATH $ENV{QT_ROOT_PATH})
endif()
set(Qt5Widgets_DIR ${QT_ROOT_PATH}/5.1.1/gcc_64/lib/cmake/Qt5Widgets/)
find_package(Qt5Widgets)

# Add the include directories for the Qt 5 libraries to the compile lines
include_directories(${Qt5Widgets_INCLUDE_DIRS})
# Use the compile definitions defined in the Qt 5 libreries module
add_definitions(-g ${Qt5Widgets_DEFINITIONS})

# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
					${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})

# Generate intermediate moc files for every header
qt5_wrap_cpp(ADDRESSBOOK_WRAPPER addressbook.h)

# Tell CMake to create the executable
add_executable(addressbook addressbook.cpp main.cpp ${ADDRESSBOOK_WRAPPER})

# Link tinyxml library statically
add_library(tinyxml STATIC IMPORTED)
set_property(TARGET tinyxml PROPERTY
                IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/tinyxml/tinyxml.a)

# Link the executable to the Qt 5 libraries.
target_link_libraries(addressbook tinyxml ${Qt5Widgets_LIBRARIES})
