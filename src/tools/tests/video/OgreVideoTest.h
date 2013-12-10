/*
 * OgreVideoTest.h
 *
 *  Created on: OCTOBER 23, 2013
 *     Company: CordobaZombie
 *      Author: Raul
 */

#ifndef OGRE_VIDEO_TEST_H_
#define OGRE_VIDEO_TEST_H_


#include <input/InputHelper.h>
#include <app_tester/AppTester.h>
#include <video/OgreVideoPlayer.h>


namespace tests {


class OgreVideoTest : public core::AppTester
{
public:

    OgreVideoTest();

    ~OgreVideoTest();

    /**
     ** @brief
     ** Load additional stuff before launching update cycle.
     **/
    void
    loadAdditionalData(void);

    /**
     ** @brief
     ** Function called every frame. Use GlobalObjects::lastTimeFrame
     **/
    void
    update();


private:

    /**
     ** @brief
     ** Interpret keyboard input to handle the video stream.
     **/
    void
    handleKeyboardInput(void);

    void
    handleMouseInput(void);





private:
    // Generic members
    float 						 mTimeFrame;
    input::InputHelper			 mInputHelper;

    // Particular members
    mm::OgreVideoPlayer			 mVPlayer;


};

}

#endif /* OGRE_VIDEO_TEST_H_ */
