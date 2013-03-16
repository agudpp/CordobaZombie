/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>

#include "GlobalObjects.h"
#include "Test.h"
#include <DotSceneLoader.h>
#include <Util.h>

#include <GameUnits/Weapon/ShortWeapon/9mm/Gun9mm.h>
#include <GameUnits/PlayerUnit/StateMachine/PlayerFSMBuilder.h>


// Add corners to a box
void Test::showCorners(const Ogre::AxisAlignedBox &b)
{
	const Ogre::Vector3 *corners = b.getAllCorners();

	for(int i = 0; i <= 7; ++i){
		Ogre::Entity *box = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		node->attachObject(box);
		node->setPosition(corners[i]);
	}

}

// handle input
void Test::handleInput(void)
{
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
		// we have to exit
		mStopRunning = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// CAMERA
//	float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
	Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
	Ogre::Real zoom = mCamController.zoom();

	// HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
	// ARROWS TO MOVE THE CAMERA
	// NOTE: we are getting the cameraPosition and modifying the position
	// without using translation, this is because we want to move always
	// in the same axis whatever be the direction of the camera.


	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	GLOBAL_CURSOR->updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
			lMouseState.X.abs <= 0)
	{
		mTranslationVec.x -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
			lMouseState.X.abs >= lMouseState.width)
	{
		mTranslationVec.x += 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_Q))
	{
		zoom += 0.05f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_E))
	{
		zoom -= 0.05f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) ||
			lMouseState.Y.abs <= 0)
	{
		mTranslationVec.z -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
			lMouseState.Y.abs >= lMouseState.height)
	{
		mTranslationVec.z += 1.0f;
	}

	if(mTranslationVec != Ogre::Vector3::ZERO)
	{
		mCamController.moveCamera(mTranslationVec);
	}
	if(zoom != mCamController.zoom()){
		mCamController.zoomCamera(zoom);
	}

	if(mKeyboard->isKeyDown(OIS::KC_R))
	{
//		mCamController.rotateCameraY(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_T))
	{
//		mCamController.rotateCameraY(Ogre::Radian(1));
	}

	if(mKeyboard->isKeyDown(OIS::KC_Z))
	{
//		mCamController.rotateCameraX(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_X))
	{
//		mCamController.rotateCameraX(Ogre::Radian(1));
	}

	const float lMouseZ = float(lMouseState.Z.rel);
	float scrollZoom = mCamController.zoom();
	if (lMouseZ > 0.0f) {
	    scrollZoom += 1.f;
	} else if (lMouseZ < 0.0f) {
	    scrollZoom -= 1.f;
	}
	if(scrollZoom != mCamController.zoom()){
        mCamController.zoomCamera(scrollZoom);
    }

	// check tracking camera
	static int lastX = 0, lastY = 0;
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
	if(lMouseState.buttonDown(OIS::MB_Right)){
	    const float factor = -0.01 * mCamController.getRotationVelocity();
	    mCamController.rotateCamera(Ogre::Radian(lMouseX * factor),
		                            Ogre::Radian(lMouseY * factor));
	}
}


void
Test::createPlayers(void)
{
    GameObject::collMng = mLevelManager.getCollisionManager();
    Ogre::Real  MAX_VEL = 180.0f;
    PlayerSMTTable *tt = PlayerFSMBuilder::build();
    PlayerUnit::setSMTransitionTable(tt);



    for(int i = 0; i < 1; i++){
        Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
        Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

        PlayerUnit *zu = new PlayerUnit;
        zu->setEntity(ent);
        zu->setSceneNode(node);
        zu->setHeight(5);
        zu->build();
        zu->setLife(9);
        node->showBoundingBox(true);
        zu->setMaxVelocity(MAX_VEL*2.0f);
        zu->setVelocity(MAX_VEL);

        sm::Vector2 p;
        p.x = 507 + 5*i;
        p.y = 788 + 30*i;
        zu->setPosition(p);
        mPlayers.push_back(zu);
    }

    Shoot::setCollisionManager(mLevelManager.getCollisionManager());
    Shoot::setUpdObjsManager(&mUpdaterManager);
    for(int i = 0; i < 10; ++i){
        Shoot *s = new Shoot;
        s->build(Ogre::Math::RangeRandom(8,16));
    }
//    Gun9mm *w = new Gun9mm;
//    w->setOwner(mPlayers.back());
//    w->setPower(1);
//    w->setSqrRange(90000.0);
//    w->setID(WP_ID_9MM);
//    w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
//    Weapon::Ammunition ammo;
//    ammo.ammo = 25;
//    w->setAmmunition(ammo);
//    mPlayers.back()->addNewWeapon(w);
}



Test::Test() :
        AppTester(false),
        mInputManager(input::InputManager::getInstance())
{
	setUseDefaultInput(false);

	mInputManager.setCameraController(&mCamController);
}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{

//	std::auto_ptr<TiXmlDocument> doc(Common::Util::loadXmlDocument("CameraLimits.xml"));
//
//	if(!doc.get()){
//		debug("Error loading camera Limits\n");
//		ASSERT(false);
//	}
//
//	GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
//	mCamController.setCamera(GLOBAL_CAMERA);

	//We load the limits of the camera and the loader configures de CameraController
//	CameraLimitsLoader* eloader = new CameraLimitsLoader(&mCamController);
//	eloader->load(doc->RootElement());


//	Ogre::AxisAlignedBox* box = eloader->getCameraBox();
//	showCorners(*box);

//	Ogre::DotSceneLoader loader;
//	loader.parseDotScene("demo.scene", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, GLOBAL_SCN_MNGR, GLOBAL_SCN_MNGR->getRootSceneNode(),"");


	GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
	mCamController.setCamera(GLOBAL_CAMERA);

	Ogre::AxisAlignedBox mb;
	mb.setMaximum(13800,13800,13800);
	mb.setMinimum(-3800,0,-3800);
	mCamController.setCameraMoveZone(mb);
	mCamController.setCameraVelocity(10.0f);

	UpdatableObject::setUpdObjsManager(&mUpdaterManager);

	// load the level
	Ogre::DotSceneLoader dsl;
	dsl.parseDotScene("cuidad01.scene", "Popular", GLOBAL_SCN_MNGR);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	static bool kPressed = false;
	static bool kPressed1 = false;
	static bool kPressed2 = false;
	static bool kPressed3 = false;
	static bool kPressed4 = false;
	static bool kPressed5 = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!kPressed){
			kPressed = true;
			mCamController.reproduceAnimation(CameraController::ANIM_VIBRATION_H);
			debug("ANIM_VIBRATION_H\n");
		}
	} else {
		kPressed = false;
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
		if(!kPressed2){
			kPressed2 = true;
			mCamController.reproduceAnimation(CameraController::ANIM_VIBRATION_M);
			debug("ANIM_VIBRATION_M\n");
		}
	} else {
		kPressed2 = false;
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_3)){
		if(!kPressed1){
			kPressed1 = true;
			mCamController.reproduceAnimation(CameraController::ANIM_VIBRATION_L);
			debug("ANIM_VIBRATION_L\n");
		}
	} else {
		kPressed1 = false;
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_9)){
		if(!kPressed3){
			kPressed3 = true;
			mCamController.setSatellitePosition();
			debug("SATELLITE mode\n");
		}
	} else {
		kPressed3 = false;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_0)){
		if(!kPressed4){
			kPressed4 = true;
			mCamController.restoreBeforeSatellite();
			debug("RESTORING SATELLITE mode\n");
		}
	} else {
		kPressed4 = false;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!kPressed5){
			kPressed5 = true;
			static Ogre::AnimationState *anim = 0;
			if (anim != 0) {
			    GLOBAL_SCN_MNGR->destroyAnimationState(anim->getAnimationName());
			}
			anim = Common::Util::getAnimationFromFile(GLOBAL_SCN_MNGR,
			                                              mCamController.getCameraSceneNode(),
			                                              "anim.xml");
			mCamController.reproduceAnimation(anim);
		}
	} else {
		kPressed5 = false;
	}

//	handleInput();


    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
    }
	mInputManager.update();

	mUpdaterManager.updateAllObjects();
	for (size_t i = 0, size = mPlayers.size(); i < size; ++i) {
	    mPlayers[i]->update();
	}
}
