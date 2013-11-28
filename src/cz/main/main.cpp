

#include <debug/DebugUtil.h>
#include <Engine.h>
#include "GameRunner.h"

// Useful defines
//
#define ENGINE_CONFIG_FILE      "EngineConfig.xml"



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif




#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI
WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char *argv[])
#endif
{
    // Log the date probably?

    // load the engine
    engine::Engine engine;
    if (!engine.loadAll(ENGINE_CONFIG_FILE)) {
        debugERROR("Error loading engine from file!\n");
        return -1;
    }

    // create the GameRunner with the engine instance
    cz::GameRunner gameRunner(engine);

    // run the game
    if (!gameRunner.run()) {
        debugERROR("The game has failed when we run it\n");
        return -2;
    }

    // everything goes fine.

    return 0;
}

#ifdef __cplusplus
}
#endif
