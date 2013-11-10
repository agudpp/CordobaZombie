
# Module headers
set(HDRS ${HDRS}
	${DEV_ROOT_PATH}/cz/hud/HUDDefines.h
	${DEV_ROOT_PATH}/cz/hud/HUDelement.h
	${DEV_ROOT_PATH}/cz/hud/HUDweapon.h
	${DEV_ROOT_PATH}/cz/hud/HUD.h
)

# Module sources
set(SRCS ${SRCS}
	${DEV_ROOT_PATH}/cz/hud/HUDweapon.cpp
	${DEV_ROOT_PATH}/cz/hud/HUD.cpp
)

# Module dynamic libraries
set(COMMON_LIBRARIES ${COMMON_LIBRARIES})

# Module static libraries
if(UNIX)
#	# ogg
#	add_library(ogg STATIC IMPORTED)
#	set_property(TARGET ogg PROPERTY  # point imported target to real file
#	             IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libogg.a)
endif(UNIX)
# TODO here we need to put Windows(TM) specific stuff

