/*
 * MiniDemoApp.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef MINIDEMOAPP_H_
#define MINIDEMOAPP_H_

#include <string.h>
#include <memory>
#include <vector>

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

#include <physics/DynamicWorld.h>
#include <types/StackVector.h>

#include "StatisticsInformer.h"
#include "Player.h"
#include "PhysicsHandler.h"
#include "PhysicObject.h"
#include "Projectile.h"
#include "SceneHandler.h"
#include "GameLogicData.h"



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
    Ogre::RenderWindow* renderWindow;
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
    static const unsigned int NUM_PROJECTILES = 25;

    enum State {
        RUNNING = 0,
        PAUSE,
        FINISH_CURRENT_GAME,
    };

    // Internal class HUD helper
    //
    class HUD {
    public:
        HUD();
        ~HUD();

        bool build(void);
        void destroy(void);
        void setVisible(bool visible);
        void setData(const GameLogicData& data);
    private:
        enum Texts {
            TOTAL_GOOD = 0,
            TOTAL_BAD,
            GOOD,
            BAD,

            T_COUNT,
        };

        Ogre::Overlay* mOverlay;
        Ogre::TextAreaOverlayElement* mTextAreas[T_COUNT];
    };

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
    void
    handlePlayerInput(float frameTime);

    // Helper internal methods, the name of the methods should be enough to
    // understand what it does.
    //
    void
    loadFloor(void);
    Projectile*
    buildPorjectile(const Ogre::Vector3& pos);


    // Executing each state
    //
    void
    runningState(void);
    void
    pauseState(void);
    void
    finishingState(void);

    // Reset the current scene
    //
    void
    resetCurrentScene(void);


private:
    physics::DynamicWorld mDynamicWorld;
    DemoData mData;
    Player mPlayer;
    PhysicsHandler mPhysicsHandler;
    SceneHandler mSceneHandler;
    Ogre::SceneNode* mFloorNode;
    std::vector<std::shared_ptr<Projectile> > mProjectiles;
    StatisticsInformer::Data mInformerData;
    GameLogicData mGameLogicData;
    GameLogicData mGameLogicDataInternal;
    State mInternalState;
    bool mRunning;
    float mTimeFrame;
    HUD mHud;
    Ogre::Overlay* mPauseOverlay;
    Ogre::Overlay* mHelpOverlay;
};

} /* namespace demo_app */
#endif /* MINIDEMOAPP_H_ */
