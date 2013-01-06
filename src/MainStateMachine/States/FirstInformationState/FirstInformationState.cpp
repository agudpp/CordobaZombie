/*
 * FirstInformationState.cpp
 *
 *  Created on: 14/08/2012
 *      Author: agustin
 */

#include "FirstInformationState.h"

#include <OgreOverlayManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreResourceManager.h>
#include <OgreTimer.h>
#include <OgreTechnique.h>

#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "GUIHelper.h"


////////////////////////////////////////////////////////////////////////////////
Ogre::Overlay *FirstInformationState::showNext(void)
{
	if(mOverlays.empty()) return 0;
	Ogre::Overlay *result = mOverlays.back();
	mOverlays.pop_back();

	debugBLUE("Showing: %s\n", result->getName().c_str());
	return result;
}

////////////////////////////////////////////////////////////////////////////////
bool FirstInformationState::exitFade(Ogre::Real ftime)
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
bool FirstInformationState::enterFade(Ogre::Real ftime)
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
bool FirstInformationState::showInfo(Ogre::Real ftime)
{
	mAccumTime += ftime;
	if(mAccumTime >= mSponsorsTime){
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
void FirstInformationState::deallocateAll(void)
{
//	for(int i = 0; i < mOverlays.size(); i++){
//		GUIHelper::fullDestroyOverlay(mOverlays[i]);
//	}
	mOverlays.clear();
//	for(int i = 0; i < mToRemoveOverlays.size(); i++){
//		GUIHelper::fullDestroyOverlay(mToRemoveOverlays[i]);
//	}
	mToRemoveOverlays.clear();

	// destroy all
	mOverlay = 0;
//	if(mFader){
//		GUIHelper::fullDestroyOverlay(mFader);
	mFader->hide();
    mFader = 0;
//	}
	mTexture = 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
FirstInformationState::FirstInformationState() :
IMainState("FirstInformationState"),
mOverlay(0),
mFader(0),
mState(STATE_FADE_IN),
mFaderTime(FADING_TIME),
mSponsorsTime(SHOWING_TIME),
mTexture(0)
{

}

////////////////////////////////////////////////////////////////////////////////
FirstInformationState::~FirstInformationState()
{

}


////////////////////////////////////////////////////////////////////////////////
void FirstInformationState::configure(Ogre::Real ft, Ogre::Real st)
{
	ASSERT(ft >= 0.0f);
	ASSERT(st >= 0.0f);

	mSponsorsTime = st;
	mFaderTime = ft;
}

void
FirstInformationState::getResources(IMainState::ResourcesInfoVec &resourcesList,
                                    const MainMachineInfo &info) const
{
    resourcesList.clear();

    IMainState::ResourcesInfo rinfo;
    rinfo.filePath = "/MainStates/FirstInfoState/resources.cfg";
    rinfo.groupNames.push_back("FirstInfoState");

    resourcesList.push_back(rinfo);
}

////////////////////////////////////////////////////////////////////////////////
void FirstInformationState::enter(const MainMachineInfo &info)
{
	debugWARNING("Hardcodeamos aca los overlays que queremos mostrar\n");
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	Ogre::Overlay *ov = om.getByName("FirstInfsOverlay/3");
	if(!ov){
		debugERROR("Error loading first overlay\n");
		return;
	}
	ov->hide();
	mOverlays.push_back(ov);

	ov = om.getByName("FirstInfsOverlay/2");
	if(!ov){
		debugERROR("Error loading second overlay\n");
		return;
	}
	ov->hide();
	mOverlays.push_back(ov);

	ov = om.getByName("FirstInfsOverlay/1");
	if(!ov){
		debugERROR("Error loading third overlay\n");
		return;
	}
	ov->hide();
	mOverlays.push_back(ov);

	// load the fader state
	ASSERT(!mFader);
	mFader = om.getByName("FaderOverlay");
	ASSERT(mFader);
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
	for(int i = 0; i < mOverlays.size(); i++){
		mOverlays[i]->setZOrder(mFader->getZOrder() - 1);	// put behind it
	}
	mState = STATE_FADE_IN;
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent FirstInformationState::update(MainMachineInfo &info)
{
	Ogre::Timer timer;
	float timeStamp = 0;
	MainMachineEvent result = MME_DONE;
	Ogre::Real frameTime = 0;
	bool press = false;

	if(mOverlays.empty()) return result;


	mOverlay = showNext();
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
			if(!press){
				press = true;

				// go to the next overlay if exists
				if (mState != STATE_FADE_OUT){
                    mState = STATE_FADE_OUT;
                    mAccumTime = 0;
                    mFader->show();
				}
			}
		} else {
			press = false;
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
			if(showInfo(frameTime)){
				mState = STATE_FADE_OUT;
				mAccumTime = 0;
				mFader->show();
			}
			break;
		case STATE_FADE_OUT:
			if(exitFade(frameTime)){
				mToRemoveOverlays.push_back(mOverlay);
				mOverlay->hide();

				// get the new one
				mOverlay = showNext();
				if(!mOverlay){
					mState = STATE_END;
				} else {
					mOverlay->show();
					mState = STATE_FADE_IN;
					mAccumTime = mFaderTime;
				}
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
			result = MME_ERROR;
			break;
		}

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		frameTime = (timer.getMilliseconds() - timeStamp) * 0.001;
	}

	return result;
}


////////////////////////////////////////////////////////////////////////////////
void FirstInformationState::exit(void)
{
	deallocateAll();
}
