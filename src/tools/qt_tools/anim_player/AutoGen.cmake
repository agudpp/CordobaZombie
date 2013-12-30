
# dependencies
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)


# This are the variables we can use / set in almost all the QtTools that we will
# develope
## SET(QtApp_MOCS ${QtApp_MOCS} )   # this will be all the .h files 
## SET(QtApp_UIS ${QtApp_UIS})      # here we will put all the ui files we use
## SET(QtApp_RCCS ${QtApp_RCCS})    # all the resources files we use

# Define the mocs and everything else
#
SET(QtApp_MOCS ${QtApp_MOCS} 
               ${DEV_ROOT_PATH}/tools/qt_tools/anim_player/QtAnimPlayer.h)
SET(QtApp_UIS ${QtApp_UIS}
              ${DEV_ROOT_PATH}/tools/qt_tools/anim_player/AnimPlayer.ui)
SET(QtApp_RCCS ${QtApp_RCCS})

# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/tools/qt_tools/anim_player/QtAnimPlayer.h
    ${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/tools/qt_tools/anim_player/QtAnimPlayer.cpp
    ${DEV_ROOT_PATH}/tools/qt_tools/anim_player/main.cpp
    ${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
)

# At the end we need to include the main OgreWidget module that contains
# all the logic for the OgreWidget
include (${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/AutoGen.cmake)

