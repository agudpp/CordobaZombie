/*
 * MiniDemoApp.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef MINIDEMOAPP_H_
#define MINIDEMOAPP_H_

#include <string.h>

#include <physics/DynamicWorld.h>

#include "StatisticsInformer.h"


// Forward
//
namespace Ogre {
class SceneManager;
class RenderWindow;
class Root;
class Camera;
}
namespace ui {
class FEManager;
class MouseCursor;
}
namespace input {
class InputHelper;
}
namespace effect {
class EffectHandler;
}

namespace rrh {
class ResourceHandler;
class ResourceGroup;
}

namespace mm {
class SoundHandler;
}

namespace demo_app {


// List all the demo data we need to execute this app
//
struct DemoData {
    Ogre::SceneManager* sceneMngr;
    Ogre::Camera* camera;
    ui::FEManager* frontEndManager;
    ui::MouseCursor* mouseCursor;
    input::InputHelper* inputHelper;
    effect::EffectHandler* effectHandler;
    const rrh::ResourceHandler* rscHandler;
    mm::SoundHandler* soundHandler;
    StatisticsInformer* informer;

    DemoData()
    {
        memset(this, 0, sizeof(DemoData));
    }
};

class MiniDemoApp
{
public:
    MiniDemoApp();
    ~MiniDemoApp();

    // @brief Set all the data needed to use here.
    //
    void
    setData(const DemoData& data);

    // @brief Get the resources to load / unload
    //
    bool
    getResourceToLoad(rrh::ResourceGroup& resource);
    bool
    getResourceToUnload(rrh::ResourceGroup& resource);

    // @brief Load / unload the MiniDemoApp
    //
    bool
    load(void);
    bool
    unload(void);

    // @brief This method will be run every frame with the last time frame.
    // @return true if we need to continue | false if we finish here.
    //
    bool
    update(float timeFrame);

private:
    // avoid copying
    MiniDemoApp(const MiniDemoApp&);
    MiniDemoApp& operator=(const MiniDemoApp&);

    // @brief Handle the camera / player.
    //

private:
    DemoData mData;
    physics::DynamicWorld mDynamicWorld;
};

} /* namespace demo_app */
#endif /* MINIDEMOAPP_H_ */
