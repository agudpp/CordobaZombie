# Define global sources here
set(HDRS ${HDRS}
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MainMenuTT.h
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MenuMainState.h
    
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/helper/MainMenuHelper.h
    
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuSubState.h
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuMainState.h
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuCreditsState.h
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuHelpState.h
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuHistoryState.h
)

set(SRCS ${SRCS}
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MainMenuTT.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/MenuMainState.cpp
    
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/helper/MainMenuHelper.cpp
    
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuSubState.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuMainState.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuCreditsState.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuHelpState.cpp
    ${DEV_ROOT_PATH}/cz/game_states/states/MenuMainState/substates/MainMenuHistoryState.cpp
)


