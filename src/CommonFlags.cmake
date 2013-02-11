#
# CommonFlags.cmake
#
#   Common compilation flags, multiplatfrom.
#
#   This file is part of the CordobaZombie© project. All rights reserved.
#


if(WIN32)
	add_definitions(-DDEBUG)
	add_definitions(-std=c++0x)  # C++11 standard
	set(COMMON_LIBRARIES boost_signals boost_system)
endif(WIN32)
if(UNIX)
	add_definitions(-DDEBUG)
	add_definitions(-std=c++0x)  # C++11 standard
	set(COMMON_LIBRARIES boost_signals boost_system)
endif(UNIX)

