/*
 * Module: 	Slide Player
 * Author: 	Raul
 * Date:	5 Sep 2012
 */

#include "DebugUtil.h"
#include "SlidePlayer.h"

////////////////////////////////////////////////////////////////////////////////
/*
 * @Overlay : The name of the slides overlay
 * @OverleyEffectConfFile : The path of the file with the effects configuration
 * for the overlays.
 *
 */
SlidePlayer::SlidePlayer(const Ogre::String &Overlay,
		const Ogre::String &OverleyEffectConfFile):
mSlide(0),
mShowing(0),
mOverlay(0),
mCenter(0),
mPrev(0),
mNext(0),
mHidden(0),
mCenterSEffect(0),
mPrevSEffect(0),
mNextSEffect(0),
mHiddenSEffect(0)
{

	//Set the overlay EFFECT manager
	OvEff::OverlayEffect::setManager(&mOvEffMngr);

	//
	Ogre::OverlayManager&  overlayManager(Ogre::OverlayManager::getSingleton());

	//
	mOverlay = overlayManager.getByName(Overlay);

	mCenter = static_cast<Ogre::PanelOverlayElement*>(
			overlayManager.getOverlayElement("SlidePlayerElement/CenterSlide"));
	mPrev 	= static_cast<Ogre::PanelOverlayElement*>(
			overlayManager.getOverlayElement("SlidePlayerElement/PrevSlide"));
	mNext 	= static_cast<Ogre::PanelOverlayElement*>(
			overlayManager.getOverlayElement("SlidePlayerElement/NextSlide"));
	mHidden = static_cast<Ogre::PanelOverlayElement*>(
			overlayManager.createOverlayElement("Panel", "Hidden"));

	if(!mCenter || !mPrev || !mNext || !mHidden){
		debugERROR("Can't create overlay elements.");
		ASSERT(false);
	}

	//Configure mHidden overlay
	mHidden->setMetricsMode(Ogre::GMM_RELATIVE);
	mHidden->setHeight(mCenter->getHeight());
	mHidden->setWidth(mCenter->getWidth());
	mHidden->setLeft(mCenter->getLeft());
	mHidden->setTop(mCenter->getTop());

	// Add the hidden panel to the overlay
	mOverlay->add2D(mHidden);

	// Hide panels
	mCenter->hide();
	mPrev->hide();
	mNext->hide();
	mHidden->hide();


	//Build slide effects for overlays using overlay effect builder.
	if(!mOEBuilder.setFilename(OverleyEffectConfFile)){
		debugERROR("Couldn't load overlay effects from xml file %s\n",
				OverleyEffectConfFile.c_str());
	}
	ASSERT(mOEBuilder.hasOpenFile());

	mCenterSEffect = static_cast<OvEff::Slide*>(
			mOEBuilder.createOverlayEffect(Ogre::String("Slide")));
	mPrevSEffect = static_cast<OvEff::Slide*>(
			mOEBuilder.createOverlayEffect(Ogre::String("Slide")));
	mNextSEffect = static_cast<OvEff::Slide*>(
			mOEBuilder.createOverlayEffect(Ogre::String("Slide")));
	mHiddenSEffect = static_cast<OvEff::Slide*>(
			mOEBuilder.createOverlayEffect(Ogre::String("Slide")));

	if(!mCenterSEffect || !mPrevSEffect || !mNextSEffect || !mHiddenSEffect)
	{
		debugERROR("No se pudo crear los overlay effects!\n");
		ASSERT(false);
	}

	//.xml doesn't specifies the velocity function for the effects
	mCenterSEffect->setFunction(&mSVFun);
	mPrevSEffect->setFunction(&mSVFun);
	mNextSEffect->setFunction(&mSVFun);
	mHiddenSEffect->setFunction(&mSVFun);

	//Get the top left positions for the slides
	mCenterTL 		= Ogre::Vector2(mCenter->getLeft(), mCenter->getTop());
	mPrevTL 		= Ogre::Vector2(mPrev->getLeft(), mPrev->getTop());
	mNextTL 		= Ogre::Vector2(mNext->getLeft(), mNext->getTop());
	mHiddenPrevTL 	= Ogre::Vector2(mPrev->getLeft()-(mCenter->getLeft()-
						mPrev->getLeft()), mPrev->getTop());
	mHiddenNextTL 	=	Ogre::Vector2(mNext->getLeft()*2.0f-mCenter->getLeft(),
						mNext->getTop());

}



////////////////////////////////////////////////////////////////////////////////
SlidePlayer::~SlidePlayer()
{

	if(!mCenterSEffect){
		delete mCenterSEffect;
	}
	if(!mPrevSEffect){
		delete mPrevSEffect;
	}
	if(!mHiddenSEffect){
		delete mHiddenSEffect;
	}
	if(!mHiddenSEffect){
		delete mHiddenSEffect;
	}
	
	//TODO full destroy de los overlay
}



////////////////////////////////////////////////////////////////////////////////
/*
 *  Place initial slides (first slide in center and second in next) if they
 *  exist.
 *
 *  @Warning: Don't use it if a seek, next, or prev has already been done. It's
 *  to be used when queuing the slides
 */
int SlidePlayer::initSlides(void)
{

	ASSERT(mShowing == 0);
	ASSERT(mSlide == 0);

	if(mSlideVec.size()>0){
		mCenter->setMaterialName(
				static_cast<const Ogre::String>(*(mSlideVec.begin())));
		mCenter->show();
	}
	if(mSlideVec.size()>1){
		mNext->setMaterialName(
				static_cast<const Ogre::String>(*(mSlideVec.begin()+1)));
		mNext->show();
	}

	//Otherwise the slides wont be shown correctly (don't know why :S)
	mPrev->setMaterialName(
			static_cast<const Ogre::String>(*(mSlideVec.begin())));

	mHidden->setMaterialName(
			static_cast<const Ogre::String>(*(mSlideVec.begin())));

	return SP_OK;
}



////////////////////////////////////////////////////////////////////////////////
int SlidePlayer::queueSlide(const Ogre::String &matName)
{
	ASSERT(Ogre::MaterialManager::getSingleton().resourceExists(matName));
	int size = mSlideVec.size();
	mSlideVec.push_back(matName);
	// If we are placing the first slides:
	if(size <= 1){
		initSlides();
	}
	return SP_OK;
}




////////////////////////////////////////////////////////////////////////////////
int SlidePlayer::queueSlides(const slideVec &slides)
{
	int size = mSlideVec.size();
	for(int i = 0; i < slides.size(); i++){
		ASSERT(Ogre::MaterialManager::getSingleton().resourceExists(slides[i]));
		mSlideVec.push_back(slides[i]);
	}

	// If we are placing the first slides:
	if(size <= 1){
		initSlides();
	}

	return SP_OK;
}




////////////////////////////////////////////////////////////////////////////////
int SlidePlayer::next(void)
{

	if(mSlide+1 >= mSlideVec.size()){

		return SP_END_REACHED;

	}else{

		mSlide++;
		return SP_OK;
	}

}


////////////////////////////////////////////////////////////////////////////////
int SlidePlayer::prev(void)
{

	if(mSlide == 0){

		return SP_END_REACHED;

	}else{

		mSlide--;
		return SP_OK;
	}

}




////////////////////////////////////////////////////////////////////////////////
int SlidePlayer::update(void)
{

	//Update overlay effects
	mOvEffMngr.update();

	if(mShowing == mSlide || isMoving()){

		return SP_OK;

	}else if(mShowing > mSlide){

		//look for previous slide

		ASSERT(mSlide < mSlideVec.size() && mSlide >= 0);

		mShowing--;

		// configure slide effects positions
		//A new slide will appear as previous:
		mPrevSEffect->setTranslationPositions(mHiddenPrevTL, mPrevTL);
		//previous slide will become center:
		mCenterSEffect->setTranslationPositions(mPrevTL, mCenterTL);
		//Slide that is at center will become next:
		mNextSEffect->setTranslationPositions(mCenterTL, mNextTL);
		//Next slide will become hidden
		mHiddenSEffect->setTranslationPositions(mNextTL, mHiddenNextTL);


		//Change pointer to overlay elements:
		Ogre::PanelOverlayElement *aux = mNext;
		mNext = mCenter;
		mCenter = mPrev;
		mPrev = mHidden;
		mHidden = aux;
		aux = 0;


		//Set Material of new slide overlay
		if(mShowing-1 >= 0){
			mPrev->setMaterialName(
					static_cast<const Ogre::String>(mSlideVec[mShowing-1]));
			mPrev->show();
		}else{
			// If we don't have more slides
			mPrev->hide();
		}


	}else{

		//look for a further slide
		ASSERT(mSlide < mSlideVec.size() && mSlide >= 0);

		mShowing++;

		// configure slide effects positions
		//Previous slide will become hidden
		mHiddenSEffect->setTranslationPositions(mPrevTL, mHiddenPrevTL);
		//Slide that is at center will become previous:
		mPrevSEffect->setTranslationPositions(mCenterTL, mPrevTL);
		//next slide will become center:
		mCenterSEffect->setTranslationPositions(mNextTL, mCenterTL);
		//A new slide will appear as next
		mNextSEffect->setTranslationPositions(mHiddenNextTL, mNextTL);

		//Change pointer to overlay elements:
		Ogre::PanelOverlayElement *aux = mPrev;
		mPrev = mCenter;
		mCenter = mNext;
		mNext = mHidden;
		mHidden = aux;
		aux = 0;

		//Set Material of new slide overlay
		if(mShowing+1 < mSlideVec.size() ){
			mNext->setMaterialName(
					static_cast<const Ogre::String>(mSlideVec[mShowing+1]));
			mNext->show();
		}else{
			// If we don't have more slides
			mNext->hide();
		}

	}

	mPrevSEffect->stop();
	mCenterSEffect->stop();
	mNextSEffect->stop();
	mHiddenSEffect->stop();
	//Attach slides to corresponding effects
	mPrevSEffect->setElement(mPrev);
	mCenterSEffect->setElement(mCenter);
	mNextSEffect->setElement(mNext);
	mHiddenSEffect->setElement(mHidden);
	//And start effects
	mPrevSEffect->start();
	mCenterSEffect->start();
	mNextSEffect->start();
	mHiddenSEffect->start();

	return SP_OK;
}



////////////////////////////////////////////////////////////////////////////////
/*
 *
 */
int SlidePlayer::seek(int index){

	if(index >= mSlideVec.size()){
		index = mSlideVec.size()-1;
	}

	if(index < 0){
		index = 0;
	}

	mSlide = index;

	return SP_OK;
}


////////////////////////////////////////////////////////////////////////////////
/*
 * Not implemented yet
 */
int SlidePlayer::playSlides(float tlapse)
{
	return SP_OK;
}
