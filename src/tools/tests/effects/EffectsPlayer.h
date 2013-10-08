/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef EffectsPlayer_H_
#define EffectsPlayer_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <utils/OrbitCamera.h>
#include <utils/SelectionHelper.h>
#include <input/InputHelper.h>
#include <global_data/GlobalData.h>
#include <cursor/MouseCursor.h>

#include <effect_handler/EffectHandler.h>
#include <fx/effects/blood/BloodParticles.h>
#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/billboards/BillboardStack.h>
#include <fx/effects/billboards/BillboardGroupHandler.h>


namespace tests {

class EffectsPlayer : public core::AppTester
{
public:
    EffectsPlayer();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~EffectsPlayer();

private:

    // @brief load billboards groups
    //
    void
    loadBillboardGroups(void);

    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    ui::MouseCursor mMouseCursor;
    tool::OrbitCamera mOrbitCamera;
    tool::SelectionHelper mSelHelper;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;
    effect::EffectHandler mEffectHandler;
    cz::BloodParticlesQueue mBloodQueue;
    cz::BillboardGroupHandler mBillboardGroups;
};

}

#endif /* EffectsPlayer_H_ */
