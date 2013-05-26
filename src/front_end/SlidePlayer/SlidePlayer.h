
/*
 * Module: 	Slide Player
 * Author: 	Raul
 * Date:	5 Sep 2012
 */


/*
 * [1]
 *
 * Slide Player Configuration:
 *
 * 		You can configure slides sizes and display positions creating a file similar
 * 	to SlidePlayer.overlay file under 'MainStates/MainMenuState/HistoryState/'
 * 	at the resource folder. You just need to change the name of the overlay, and
 * 	adjust values for the elements variables as you wish.
 * 		You can also set the transition effect options in the creating a file
 * 	similar to SlidePlayerOverlayEffect.xml file under
 * 	'MainStates/MainMenuState/HistoryState/' at the resource folder.
 * 		You can then pass ass parameters the name of the new overlay and the
 * 	name of the effect configuration file to the class constructor. Be sure
 * 	that Ogre recognizes the new Overlay resource when you define it.
 *
 *
 * [2]
 *
 * Require:
 *
 * 	SlidePlayer needs an "overlay effect manager" running "from the outside"
 * 	to update its slides.
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


namespace f_e {

const float PI = 3.1415f;

class SlideVelFun: public f_e::Slide::VelFunction
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
	 * @param Overlay:
	 * 	Name of the slide player overlay. Should be available to the Ogre
	 * 	Overlay Manager, and should be well defined as described in [1].
	 * @param OverlayEffectConfFile:
	 * 	Name of the effect overlay effect configuration file the player should
	 *  use.
	 */
	SlidePlayer(const Ogre::String &Overlay,
				const Ogre::String &OverleyEffectConfFile);

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
	 * Load slides with base name 'sldBaseName' into the slide player.
	 * Slides must be present in an already loaded .material file.
	 */
	int loadSlides(const char *sldBaseName);

	/*
	 * Move to next slide.
	 * 	@Return SP_OK on success; SP_END_REACHED if the end has been reached and
	 * we are not able to go further.
	 */
	int next();

	/*
	 * Move to previous slide.
	 * 	@Return SP_OK on success; SP_END_REACHED if the end has been reached and
	 * we are not able to go further.
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
	 * To update the slide player each frame.
	 */
	int update(void);


	/*
	 * Ask if the slides are moving in this moment.
	 */
	inline bool isMoving(void);


	/**
	 * If has at least one slide
	 */
	inline bool hasSlides(void);



private:

	//
	SlideVelFun 						mSVFun;
	//
	f_e::OverlayEffectBuilder			mOEBuilder;
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
	f_e::Slide						*mCenterSEffect;
	f_e::Slide						*mPrevSEffect;
	f_e::Slide						*mNextSEffect;
	f_e::Slide						*mHiddenSEffect;
	// Top left positions for slides
	Ogre::Vector2						mCenterTL;
	Ogre::Vector2						mPrevTL;
	Ogre::Vector2						mHiddenPrevTL;
	Ogre::Vector2						mNextTL;
	Ogre::Vector2						mHiddenNextTL;

private:

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
//	mCenter->show();
//	mPrev->show();
//	mNext->show();
//	mHidden->show();

	return SP_OK;
}


////////////////////////////////////////////////////////////////////////////////
inline int SlidePlayer::hide(void)
{
//	mCenter->hide();
//	mPrev->hide();
//	mNext->hide();
//	mHidden->hide();
	mOverlay->hide();

	return SP_OK;
}

inline bool SlidePlayer::hasSlides(void)
{
	return mSlideVec.size() > 0;
}

}
#endif // SLIDE_PLAYER_H_
