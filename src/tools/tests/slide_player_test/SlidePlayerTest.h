/*
 * SlidePlayerTest.h
 *
 *  Created on: 1/11/2013
 *      Author: agustin
 */

#ifndef SlidePlayerTest_H_
#define SlidePlayerTest_H_


#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <ResourceHandler.h>
#include <frontend/FEManager.h>
#include <cursor/MouseCursor.h>
#include <input/InputHelper.h>
#include <effect_handler/EffectHandler.h>
#include <helpers/slide_player/SlidePlayer.h>


namespace tests {

class SlidePlayerTest : public core::AppTester
{
public:
    SlidePlayerTest();

    ~SlidePlayerTest();

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
    configureSlidePlayer(void);


private:
    ui::MouseCursor mCursor;
    float mTimeFrame;
    input::InputHelper mInputHelper;
    rrh::ResourceHandler mRcHandler;
    ui::FEManager mFrontEnd;
    effect::EffectHandler mEffectHandler;
    cz::SlidePlayer mSlidePlayer;


};

}

#endif /* SlidePlayerTest_H_ */
