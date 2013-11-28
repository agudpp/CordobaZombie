/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef FrontEndTester_H_
#define FrontEndTester_H_

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
#include <frontend/FEManager.h>
#include <frontend/FEInfo.h>
#include <frontend/element/button/FESimpleButton.h>
#include <frontend/effects/FESimpleButtonEffect.h>
#include <overlay_effects/SlideOverlayEffect.h>
#include <overlay_effects/AlphaOverlayEffect.h>



namespace tests {

class FrontEndTester : public core::AppTester
{
public:
    FrontEndTester();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~FrontEndTester();

private:

    // @brief Load some buttons and front end elements
    //
    void
    loadFrontEndElements(void);

    // @brief Configure effects
    //
    void
    configureEffects(void);

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
    ui::FEManager mFrontEndManager;
    ui::FESimpleButton* mButton;
    ui::AlphaOverlayEffect mAlphaEffect;
    ui::SlideOverlayEffect mSlideEffect;
    ui::FESimpleButtonEffect mButtonEffect;
};

}

#endif /* FrontEndTester_H_ */
