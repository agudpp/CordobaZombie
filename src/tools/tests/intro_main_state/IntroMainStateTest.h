/*
 * ResourcesLoadingTest.h
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */

#ifndef IntroMainStateTest_H_
#define IntroMainStateTest_H_


#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <game_states/IMainState.h>
#include <ResourceHandler.h>
#include <frontend/FEManager.h>
#include <cursor/MouseCursor.h>
#include <input/InputHelper.h>
#include <effect_handler/EffectHandler.h>
#include <video/OgreVideoPlayer.h>


namespace tests {

class IntroMainStateTest : public core::AppTester
{
public:
	IntroMainStateTest();

	~IntroMainStateTest();

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
    ui::MouseCursor mCursor;
    float 					mTimeFrame;
    input::InputHelper 		mInputHelper;
    cz::IMainState			*mIntroMainState;
    rrh::ResourceHandler	mRcHandler;
    ui::FEManager           mFrontEnd;
    effect::EffectHandler   mEffectHandler;
    mm::OgreVideoPlayer mOgreVideoPlayer;


};

}

#endif /* IntroMainStateTest_H_ */
