
/*
 * Module: 	Slide Player
 * Author: 	Raul
 * Date:	5 Sep 2012
 */


/*
 * This is the slide player, thought to be used for the history state of the
 * main state machine. It display slides and hides not displayed ones out of
 * out of the screen.
 * @Configuration:
 * 	You can configure slides sizes and display positions modifying the
 * 	SlidePlayer.overlay file under 'MainStates/HistoryState/' at the resource
 * 	folder.
 * 	You can define the slides names and images at the SlidePlayer.material
 * 	under 'MainStates/HistoryState/' at the resource folder.
 * 	Finally you can set the transition effect options in the
 * 	SlidePlayerOverlayEffect.xml file under 'MainStates/HistoryState/' at the
 * 	resource folder.
 */



//TODO cargar opciones por defecto cuando no esten los .xml

#ifndef SLIDE_PLAYER_H_
#define SLIDE_PLAYER_H_

#include <vector>
#include "OverlayEffectBuilder.h"
#include "OverlayEffectManager.h"
#include "Ogre.h"
#include "OgrePrerequisites.h"
#include "OverlayEffect.h"
#include "Slide.h"
#include "OgrePanelOverlayElement.h"
#include "OgreOverlayManager.h"



const float PI = 3.1415;

class SlideVelFun: public OvEff::Slide::VelFunction
{
public:
	float operator()(float x) const
	{
		x *= 3.1415f;
		return (std::sin(x-(PI/2.0f))+1.0f)/2.0f;
	}
};





class SlidePlayer{


public:


	typedef std::vector<Ogre::String> slideVec;

	enum{
		SP_END_REACHED = -2,
		SP_ERROR = -1,
		SP_OK = 0,
	};

public:

	/*
	 *
	 */
	SlidePlayer();

	/*
	 *
	 */
	~SlidePlayer();

	/*
	 * Show the player.
	 */
	inline int show(void);

	/*
	 * Hide the player.
	 */
	inline int hide(void);

	/*
	 * Queue a single slide into the slide player.
	 */
	int queueSlide(const Ogre::String &matName);

	/*
	 * Queue a set of slides into the slide player.
	 */
	int queueSlides(const slideVec &slides);

	/*
	 * Move to next slide.
	 */
	int next();

	/*
	 * Move to previous slide.
	 */
	int prev();

	/*
	 * Move to the i'th slide.
	 */
	int seek(int index);

	/*
	 * Plays the slides one after the other, showing each of them during
	 * 'tlapse' seconds.
	 */
	int playSlides(float tlapse);

	/*
	 * To update the slide player each frame
	 */
	int update(void);

private:

	//
	SlideVelFun 						mSVFun;
	//
	OvEff::OverlayEffectBuilder			mOEBuilder;
	//Vector of slides to show:
	slideVec				 			mSlideVec;
	//The slide the user wants to see:
	int									mSlide;
	//Points to the actually showed slide:
	int				 					mShowing;
	//
	Ogre::Overlay 						*mOverlay;
	//
	Ogre::PanelOverlayElement			*mCenter;
	Ogre::PanelOverlayElement			*mPrev;
	Ogre::PanelOverlayElement			*mNext;
	Ogre::PanelOverlayElement			*mHidden;
	//
	OvEff::Slide						*mCenterSEffect;
	OvEff::Slide						*mPrevSEffect;
	OvEff::Slide						*mNextSEffect;
	OvEff::Slide						*mHiddenSEffect;
	// Top left positions for slides
	Ogre::Vector2						mCenterTL;
	Ogre::Vector2						mPrevTL;
	Ogre::Vector2						mHiddenPrevTL;
	Ogre::Vector2						mNextTL;
	Ogre::Vector2						mHiddenNextTL;
	//TODO uso mi propio overlay manager???
	OvEff::OverlayEffectManager			mOvEffMngr;

private:

	inline bool isMoving(void);

	/*
	 * Show initial slides
	 */
	int initSlides(void);
};





////////////////////////////////////////////////////////////////////////////////
inline bool SlidePlayer::isMoving(void)
{
	if(mCenter == 0){
		return false;
	}else{
		return (mCenter->getLeft() != mCenterTL.x);
	}
}


////////////////////////////////////////////////////////////////////////////////
inline int SlidePlayer::show(void)
{
	mOverlay->show();
	return SP_OK;
}


////////////////////////////////////////////////////////////////////////////////
inline int SlidePlayer::hide(void)
{
	mOverlay->hide();
	return SP_OK;
}


#endif // SLIDE_PLAYER_H_
