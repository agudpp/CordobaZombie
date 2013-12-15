include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/os_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/engine/resources_config_dialog/AutoGen.cmake)

# Define global sources here
set(HDRS ${HDRS}
)

set(SRCS ${SRCS}
)

################################################################################
##                       ConfigDialog specific stuff                         ###
################################################################################

if(UNIX)
#add_library(LinearMath STATIC IMPORTED)
## point the imported target at the real file
#set_property(TARGET LinearMath PROPERTY
#                IMPORTED_LOCATION ${THIRD_PARTY_LIBS}/lib/libLinearMath.a)
endif(UNIX)
# TODO here we need to put Windows(TM) specific stuff
