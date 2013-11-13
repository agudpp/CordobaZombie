/*
 * VideoTest.h
 *
 *  Created on: OCTOBER 3, 2013
 *     Company: CordobaZombie
 *      Author: Monti, Raul E.
 */

#ifndef VIDEO_TEST_H_
#define VIDEO_TEST_H_


#include <input/InputHelper.h>
#include <app_tester/AppTester.h>
#include <VideoPlayer.h>
#include <VideoBuffer.h>


namespace tests {


class OgreVideoScreen : public mm::VideoBuffer
{

public:


	/*
	 * Parameters define the relative size position and size of the Ogre
	 * screen in the monitor. Read the Ogre::Rectangle2D::setCorners
	 * description for more information.
	 */
	OgreVideoScreen(Ogre::Real left,
					Ogre::Real top,
			 	 	Ogre::Real right,
			 	 	Ogre::Real bottom,
			 	 	Ogre::SceneManager *SCN_MNGR,
			 	 	unsigned int height,
			 	 	unsigned int width);


	/*
	 *
	 */
	virtual
	~OgreVideoScreen(void);


	/*
	 * Place data from 'source' into the buffer. 'offset' is the starting point
	 * in the buffer counted as number of bytes. 'size' indicates how much bytes
	 * should we write into the buffer. 'format' identifies the image data
	 * format as it is in 'source'.
	 */
	int
	fillBuffer(unsigned char* source = 0,
					Format format = RGB,
					unsigned int size = 0);


	/*
	 * Set corresponding size texture to the Ogre Screen.
	 */
	int
	resize(unsigned int h, unsigned int w);


private:

	Ogre::Rectangle2D 			*mScreen;
	Ogre::SceneNode 			*mScreenNode;
	Ogre::TexturePtr 			mRttTexture;
	Ogre::MaterialPtr 			mRenderMaterial;
	unsigned int				mHeight;
	unsigned int				mWidth;


};





class VideoTest : public core::AppTester
{
public:

    VideoTest();

    ~VideoTest();

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
    mm::VideoPlayer				 *mVPlayer;
    OgreVideoScreen 			 *mVideoScreen;


};

}

#endif /* SOUND_TEST_H_ */
