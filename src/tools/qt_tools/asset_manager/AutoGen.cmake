# include here all the project modules dependencies
#
include(${DEV_ROOT_PATH}/core/debug/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/ogre_utils/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/xml/AutoGen.cmake)
include(${DEV_ROOT_PATH}/core/asset/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/AutoGen.cmake)
include(${DEV_ROOT_PATH}/coll_physics/collisions/helpers/AutoGen.cmake)
include(${DEV_ROOT_PATH}/world_object/AutoGen.cmake)


# This are the variables we can use / set in almost all the QtTools that we will
# develop.
# Here we will put all the mocs or qt headers files (.h with qt stuff only) of our 
# Qt tool
#
SET(QtApp_MOCS ${QtApp_MOCS} 
               ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/QtAssetManager.h
               ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/ConfigAssetGUI.h)

# All the .ui files of our tool
#
SET(QtApp_UIS ${QtApp_UIS}
              ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/AssetManager.ui
              ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/ConfigAsset.ui)

# All the .qrc files of our tool (resource files, probably none)
#
SET(QtApp_RCCS ${QtApp_RCCS})



# Define global sources and headers here (.h, .cpp files) here. Note that we are
# probably redefining all the headers here but is necessary.
#
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/QtAssetManager.h
    ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/ConfigAssetGUI.h
    ${DEV_ROOT_PATH}/tools/utils/OrbitCamera.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/QtAssetManager.cpp
    ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/ConfigAssetGUI.cpp
    ${DEV_ROOT_PATH}/tools/qt_tools/asset_manager/main.cpp
    ${DEV_ROOT_PATH}/tools/utils/OrbitCamera.cpp
)

# At the end we need to include the main OgreWidget module that contains
# all the logic for the OgreWidget
# This should be here at the end.
include (${DEV_ROOT_PATH}/tools/qt_tools/ogre_widget/AutoGen.cmake)
