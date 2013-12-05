# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/MiniDemoApp.h
#   ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/ObjectiveObject.h
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/PhysicObject.h
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/PhysicsHandler.h
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/Player.h
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/SceneHandler.h
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/StatisticsInformer.h
#   ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/WorldObject.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/MiniDemoApp.cpp
#   ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/ObjectiveObject.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/PhysicsHandler.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/Player.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/SceneHandler.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/StatisticsInformer.cpp
#   ${DEV_ROOT_PATH}/cz/game_states/states/PlayingMainState/AngryZombie/WorldObject.cpp
)

include(${DEV_ROOT_PATH}/coll_physics/physics/AutoGen.cmake)