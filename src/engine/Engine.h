/*
 * Engine.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef ENGINE_H_
#define ENGINE_H_


#include <string>

#include <types/StackVector.h>
#include <modules/IModuleLoader.h>

#include "EngineLoader.h"

// forward
//
namespace Ogre {
class SceneManager;
class RenderWindow;
class Root;
class Camera;
}

namespace mm {
class SoundHandler;
class OgreVideoPlayer;
}

namespace rrh {
class ResourceHandler;
}

namespace OIS {
class Mouse;
class Keyboard;
class InputManager;
}

namespace engine {

class Engine
{
public:
    // Helper containers structures
    //

    // Ogre stuff
    //
    struct OgreData {
        Ogre::SceneManager* sceneMngr;
        Ogre::RenderWindow* renderWindow;
        Ogre::Root* root;
        Ogre::Camera* camera;

        OgreData() :
            sceneMngr(0)
        ,   renderWindow(0)
        ,   root(0)
        ,   camera(0)
        {}
    };

    // SoundSystem information
    //
    struct SoundData {
        mm::SoundHandler* handler;

        SoundData() :
            handler(0)
        {}
    };

    // Video information
    //
    struct VideoData {
        mm::OgreVideoPlayer* player;

        VideoData() :
            player(0)
        {}
    };

    // Resource information
    //
    struct ResourcesData {
        rrh::ResourceHandler* handler;

        ResourcesData() :
            handler(0)
        {}
    };

    // Input system, this will configure the static global input::Mouse and
    // input::Keyboard so it is not necessary any information.
    // We will put the global OIS data
    //
    struct InputData {
        OIS::Mouse* mouse;
        OIS::Keyboard* keyboard;
        OIS::InputManager* manager;

        InputData() :
            mouse(0)
        ,   keyboard(0)
        ,   manager(0)
        {}
    };

public:
    Engine();
    ~Engine();

    // @brief Get all the associated information to this engine instance.
    //
    inline OgreData&
    ogreData(void);
    inline SoundData&
    soundData(void);
    inline VideoData&
    videoData(void);
    inline ResourcesData&
    resourcesData(void);
    inline InputData&
    inputData(void);

    // @brief Load an configure the engine from a configuration file path.
    //        This method will load and configure each one of the modules
    //        associated to the engine. Note that the instances will be tracked
    //        over all the life time of this engine instance (so, do not delete
    //        the engine during the execution of the game).
    // @path configPath     The path to the configuration file
    // @return true on success and if all the modules were loaded right | false
    //         on any error (and an error message will be shown).
    //
    bool
    loadAll(const std::string& configPath);

    // @brief Shut it down the entire engine (and all the modules).
    //        After calling this method you will be not able to do anything with
    //        the before allocated modules.
    // @return true on success | false on error
    //
    bool
    unloadAll(void);

private:
    EngineLoader mLoader;
    core::StackVector<IModuleLoader*, 10> mLoaders;
    OgreData mOgreData;
    SoundData mSoundData;
    VideoData mVideoData;
    ResourcesData mResourcesData;
    InputData mInputData;
};









////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline Engine::OgreData&
Engine::ogreData(void)
{
    return mOgreData;
}
inline Engine::SoundData&
Engine::soundData(void)
{
    return mSoundData;
}
inline Engine::VideoData&
Engine::videoData(void)
{
    return mVideoData;
}
inline Engine::ResourcesData&
Engine::resourcesData(void)
{
    return mResourcesData;
}
inline Engine::InputData&
Engine::inputData(void)
{
    return mInputData;
}

} /* namespace engine */
#endif /* ENGINE_H_ */
