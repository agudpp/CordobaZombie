/*
 * SponsorsState.cpp
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#include <OgreWindowEventUtilities.h>
#include <OgreTimer.h>
#include <OgreOverlayManager.h>

#include "GUIHelper.h"
#include "DebugUtil.h"
#include "SponsorsState.h"
#include "GlobalObjects.h"




////////////////////////////////////////////////////////////////////////////////
bool SponsorsState::exitFade(Ogre::Real ftime)
{
	ASSERT(mTexture);

	mAccumTime += ftime;
	const Ogre::Real faderStep = mAccumTime / mFaderTime;
	mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
					Ogre::LBS_TEXTURE, faderStep);
	if(mAccumTime >= mFaderTime){
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool SponsorsState::enterFade(Ogre::Real ftime)
{
	ASSERT(mTexture);

	mAccumTime -= ftime;
	const Ogre::Real faderStep = mAccumTime / mFaderTime;
	mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
					Ogre::LBS_TEXTURE, faderStep);
	if(mAccumTime <= 0){
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool SponsorsState::showSponsors(Ogre::Real ftime)
{
	mAccumTime += ftime;
	if(mAccumTime >= mSponsorsTime){
		return true;
	}

	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SponsorsState::SponsorsState() :
IMainState("SponsorsState"),
mOverlay(0),
mFader(0),
mState(STATE_FADE_IN),
mFaderTime(1.5f),
mSponsorsTime(5.0f),
mTexture(0)
{

}

////////////////////////////////////////////////////////////////////////////////
SponsorsState::~SponsorsState()
{
	exit();
}

////////////////////////////////////////////////////////////////////////////////
void SponsorsState::configure(Ogre::Real ft, Ogre::Real st)
{
	ASSERT(ft >= 0.0f);
	ASSERT(st >= 0.0f);

	mSponsorsTime = st;
	mFaderTime = ft;
}

////////////////////////////////////////////////////////////////////////////////
void
SponsorsState::getResources(IMainState::ResourcesInfoVec &resourcesList)
{
    resourcesList.clear();

    IMainState::ResourcesInfo rinfo;
    rinfo.filePath = "/MainStates/SponsorsState/resources.cfg";
    rinfo.groupNames.push_back("SponsorsState");

    resourcesList.push_back(rinfo);
}

////////////////////////////////////////////////////////////////////////////////
void SponsorsState::enter(const MainMachineInfo &info)
{
	// load the fader state
	ASSERT(mFader == 0);
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	mFader = om.getByName("FaderOverlay");
	ASSERT(mFader != 0);
	Ogre::PanelOverlayElement *panel = static_cast<Ogre::PanelOverlayElement *>(
					mFader->getChild("FaderOverlay/Background"));
	ASSERT(panel);
	Ogre::MaterialPtr mat = panel->getMaterial();
	ASSERT(!mat.isNull());
	mTexture = mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
						Ogre::LBS_TEXTURE, 0);
	ASSERT(mTexture);

	// load the  sponsors Overlay
	ASSERT(!mOverlay);
	mOverlay = om.getByName("SponsorsOverlay");
	ASSERT(mOverlay);
	mOverlay->setZOrder(mFader->getZOrder() - 1);	// put behind it

	mState = STATE_FADE_IN;
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent SponsorsState::update(MainMachineInfo &info)
{
	Ogre::Timer timer;
	float timeStamp = 0;
	MainMachineEvent result = MME_DONE;
	Ogre::Real frameTime = 0;
	float press = false;

	// check we have loaded both overlays
	ASSERT(mFader);
	ASSERT(mOverlay);

	// here is the main loop
	bool running = true;
	mAccumTime = mFaderTime;
	mFader->show();
	mOverlay->show();
	while(running) {
		timeStamp = timer.getMilliseconds();

		GLOBAL_KEYBOARD->capture();
		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
			break;
		}

		// depending on the state we call the function
		switch(mState){
		case STATE_FADE_IN:
			if(enterFade(frameTime)){
				mState = STATE_SPONSORS;
				mAccumTime = 0;
				mFader->hide();
			}
			break;
		case STATE_SPONSORS:
			if(showSponsors(frameTime)){
				mState = STATE_FADE_OUT;
				mAccumTime = 0;
				mFader->show();
			}
			break;
		case STATE_FADE_OUT:
			if(exitFade(frameTime)){
				mState = STATE_END;
			}
			break;
		case STATE_END:
			running = false;
			break;
		default:
			ASSERT(false);
		}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			result = MME_DONE; //TODO: poner un erro real aca
			break;
		}

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		frameTime = (timer.getMilliseconds() - timeStamp) * 0.001;
	}

	return result;
}


////////////////////////////////////////////////////////////////////////////////
void SponsorsState::exit(void)
{
	// destroy all
	if(mOverlay){
		GUIHelper::fullDestroyOverlay(mOverlay);
		mOverlay = 0;
	}
	if(mFader){
		GUIHelper::fullDestroyOverlay(mFader);
		mFader = 0;
	}
	mTexture = 0;
}

