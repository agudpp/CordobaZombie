/*
 * ResourcesLoadingTest.h
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */

#ifndef MainMenuStateTest_H_
#define MainMenuStateTest_H_

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <game_states/IMainState.h>
#include <ResourceHandler.h>

#include <input/InputHelper.h>

#include <cursor/MouseCursor.h>
#include <frontend/FEManager.h>
#include <frontend/FEInfo.h>
#include <frontend/element/button/FESimpleButton.h>
#include <effect_handler/EffectHandler.h>

namespace tests {

class MainMenuStateTest : public core::AppTester
{
public:
    MainMenuStateTest();

    ~MainMenuStateTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    bool
    initializeState(void);

    bool
    closeState(void);

private:
    void
    loadButtonFromOverlay(void);
    void
    dummy(ui::FESimpleButton*, ui::FESimpleButton::Event);
private:

    ui::MouseCursor mMouseCursor;
    float mTimeFrame;
    input::InputHelper mInputHelper;
    cz::IMainState *mMainState;
    rrh::ResourceHandler mRcHandler;
    ui::FEManager mFrontEndManager;
    effect::EffectHandler mEffectHandler;

};

}

#endif /* MainMenuStateTest_H_ */
