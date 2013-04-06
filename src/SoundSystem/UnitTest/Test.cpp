/*
 * Test.h
 *
 *  Created on: May 8, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>  // time
#include <map>

#include <OgreSceneNode.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include "Test.h"
#include "DebugUtil.h"

#include "GameUnitDefines.h"
#include "CircularFormation.h"
#include "SMTransitionTable.h"
#include "PlayerFSMBuilder.h"
#include "ZombieFSMBuilder.h"
#include "ZombieIdleState.h"
#include "ZombieDefs.h"
#include "Weapon.h"
#include "WeaponIDs.h"
#include "ShortWeapon.h"
#include "Gun9mm.h"
#include "ProximityBomb.h"
#include "BillboardBatery.h"
#include "GlobalObjects.h"
#include "CollectableObject.h"
#include "CollectableObjTypes.h"
#include "BackpackItem.h"
#include "WeaponBackpackItem.h"
#include "BackpackDefines.h"


#if defined(_WIN32) || defined(CYGWIN)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif


#include "SoundManager.h"
#include "LSoundSource.h"


const float Test::TIME_UPDATE_PERIOD = 1.0;


/******************************************************************************/
/*******************    GLOBAL VARIABLES AND DEFINES    ***********************/


/* Posiciones iniciales de los players. */
const sm::Vector2 pos1(521.0, 1188.0);	// coral
const sm::Vector2 pos2(1107.0, 1030.0);	// cardenal

/* Ogre sounds resource name */
const Ogre::String soundsGroupName("Sounds");

/* Players y sus SoundAPIs */
Ogre::Entity *coral = 0;
Ogre::Entity *cardenal = 0;
Ogre::SceneNode* coralNode = 0;
Ogre::SceneNode* cardenalNode = 0;
SoundAPI* coralSoundAPI = 0;
SoundAPI* cardenalSoundAPI = 0;

/* Atributos del player (coral) */
#define  NUM_LIFE  (1<<27)  // 128 M
#define  NUM_AMMO  (1<<5)   // 32

/* Cantidad de Zombies producidos en cada factory */
#define  NUM_ZOMBIES_1  10
#define  NUM_ZOMBIES_2  10

/* Cantidad de buffers y sources para direct play */
#define  NUM_LBUFFERS  20
#define  NUM_LSOURCES  60

/* Cantidad de buffers y sources para streaming */
#define  NUM_SBUFFERS  5
#define  NUM_SSOURCES  NUM_SBUFFERS

// Archivos de audio
#define  NUM_SFILES      6
#define  START_PLSOUNDS  2
const char *audioFile[NUM_SFILES] = {
		"fxA20.ogg",
		"Siren.ogg",
		"roar.wav",		// playlists sound #1
		"fxZ7.ogg",		// playlists sound #2
		"fxZ9.ogg",		// playlists sound #3
		"fxM2.ogg"		// playlists sound #4
};

// Playlists
const Ogre::String playlist1("lista1");
const Ogre::String playlist2("lista2");


// Tiempo para fadings (in & out) de sonidos.
#define  FADE_TIME  2.5f

/******************************************************************************/
/***********************    AUXILIARY FUNCTIONS     ***************************/


/* Sleep for sec seconds and nsec nanoseconds (1 sec = 10^9 nsec)
 * Default behaviour is 0 sec sleep (i.e. yield processor only)
static inline void
sleep(time_t sec=0, long nsec=0)
{
	struct timespec wtime;
	wtime.tv_sec  = sec;
	wtime.tv_nsec = nsec;
	nanosleep(&wtime, NULL);
}
*/


static void
printDevices(void)
{
	SoundManager& sMgr=SoundManager::getInstance();

	std::cout << "Available sound devices:" << std::endl;
	std::vector<std::string> soundDevices = sMgr.getAvailableSoundDevices();
	for (unsigned int i=0 ; i < soundDevices.size() ; i++) {
		std::cout << "  ¤ " << soundDevices[i] << std::endl;
	}
	std::cout << "Using sound device:" << sMgr.getSoundDevice() << std::endl;
}



void Test::handleInput()
{
	static SoundManager& sMgr(SoundManager::getInstance());
	static bool mousePressed(false);
	static bool keyPressed(false);
	static std::vector<SSplayback> state(4, SSplayback::SS_PLAYING);


	// MOUSE

	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed) {
			mousePressed = true;
			Ogre::Vector3 v;

			// check if we are getting a player
			static CollisionResult cr;
			static PlayerUnit *pu = 0;

			// first check if we have a player selected
			if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_LSHIFT)){
				if(pu){
					pu->objectUnselected();
					pu = 0;
				}
				goto exit_mouse_input;
			}
			// else...

			mLevelManager.getRaycastManger()->getPoint(mMouseCursor.getXRelativePos(),
					mMouseCursor.getYRelativePos(), v);
			mLevelManager.getCollisionManager()->getCollisionObjects(
					sm::Point(v.x, v.z), COL_FLAG_UNIT_PLAYER ,cr);

			if(!cr.empty()){
				// get the player
				pu = static_cast<PlayerUnit *>(cr.front()->userDefined);
				pu->objectSelected();
			} else {
				if(pu){
					pu->plantBomb(mBomb, sm::Vector2(v.x,v.z));
				}
			}
		}
	} else {
		if (mousePressed) {
			mousePressed = false;
		}
	}
	exit_mouse_input:


	// KEYBOARD

	// Test collect object
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_C)){
		if (!keyPressed) {
			keyPressed = true;
			testCollectObject();
		}

	// Zombies creation
	} else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)){
		if (!keyPressed) {
			keyPressed = true;
			testStart();
		}

	// Zombies attack mode
	} else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_E)){
		if (!keyPressed) {
			keyPressed = true;
			testEngageEveryone();
		}

	// Toogle play/pause of all sounds.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_NUMPAD0)) {
		if (!keyPressed) {
			keyPressed = true;
			if (state[0] == SSplayback::SS_PLAYING) {
				mSoundHandler.globalPause();
				state[0] = SSplayback::SS_PAUSED;
				debugBLUE("Global sounds PAUSED.%s", "\n");
			} else {
				mSoundHandler.globalPlay();
				state[0] = SSplayback::SS_PLAYING;
				debugBLUE("Global sounds PLAY.%s", "\n");
			}
		}

	// Toogle play/pause of units sounds.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_NUMPAD1)) {
		if (!keyPressed) {
			keyPressed = true;
			if (state[1] == SSplayback::SS_PLAYING) {
				pauseUnitsSounds();
				state[1] = SSplayback::SS_PAUSED;
				debugBLUE("Units' sounds PAUSED.%s", "\n");
			} else {
				playUnitsSounds();
				state[1] = SSplayback::SS_PLAYING;
				debugBLUE("Units' sounds PLAY.%s", "\n");
			}
		}

	// Toogle play/pause of environmental music.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_NUMPAD2)) {
		if (!keyPressed) {
			keyPressed = true;
			if (state[2] == SSplayback::SS_PLAYING) {
				pauseEnvSounds();
				state[2] = SSplayback::SS_PAUSED;
				debugBLUE("Environmental music PAUSED.%s", "\n");
			} else {
				playEnvSounds();
				state[2] = SSplayback::SS_PLAYING;
				debugBLUE("Environmental music PLAY.%s", "\n");
			}
		}

	// Restart all sounds.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)) {
		if (!keyPressed) {
			keyPressed = true;
			mSoundHandler.globalRestart();
			debugBLUE("Global sounds RESTARTED.%s", "\n");
		}

	// Stop all sounds.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_NUMPADENTER)) {
		if (!keyPressed) {
			keyPressed = true;
			mSoundHandler.globalStop();
			debugBLUE("Global sounds STOPPED.%s", "\n");
		}

	// Toogle fade in/out of all sounds.
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_NUMPAD3)) {
		if (!keyPressed) {
			keyPressed = true;
			if (state[3] != SSplayback::SS_FADING_OUT_AND_PAUSE) {
				mSoundHandler.globalFadeOut(FADE_TIME);
				state[3] = SSplayback::SS_FADING_OUT_AND_PAUSE;
				debugBLUE("Global sounds FADING OUT (%.2f seconds)\n", FADE_TIME);
			} else {
				mSoundHandler.globalFadeIn(FADE_TIME);
				state[3] = SSplayback::SS_FADING_IN;
				debugBLUE("Global sounds FADING IN (%.2f seconds)\n", FADE_TIME);
			}
		}

	// No relevant key press.
	} else {
		if (keyPressed) {
			keyPressed = false;
		}
	}
}


bool
Test::performMouseRay(Ogre::Vector3 &v)
{
	// set up the ray
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
			mMouseCursor.getYRelativePos());

	/* Check whether the ray intersects our plane.
	 * If it does, intersects() will return "true" together with the distance
	 * (a Real value) along the ray where the intersection is. */
	std::pair<bool, Ogre::Real> result = mouseRay.intersects(mGlobalPlane);

	if (result.first)
	{
		/* If the ray intersects the plane, we have a distance value
		 * telling us how far from the ray origin the intersection took place.
		 * The last thing we need is the point of the intersection.
		 * Ray provides us getPoint() function which returns a point
		 * along the ray, supplying it with a distance value.  */
		// get the point where the intersection is
		v = mouseRay.getPoint(result.second);
		return true;
	}

	return false;
}


/*
 * @brief
 * Various initializations for the ingame SoundSystem control interface
 */
static void
soundInitMisc(SoundHandler& SH)
{
	SSerror err(SSerror::SS_INTERNAL_ERROR);
	SSplayback pstate(SSplayback::SS_NONE);
	std::vector<Ogre::String> soundsList;

	// Put on some environmental music.
	testBEGIN("Iniciando reproducción del sonido ambiente \"%s\".\n", audioFile[0]);
	err = SoundManager::getInstance().playEnvSound(audioFile[0], DEFAULT_ENV_GAIN, true);
	if (err == SSerror::SS_NO_ERROR) {
		ASSERT(SoundManager::getInstance().isPlayingEnvSound(audioFile[0]));
		testSUCCESS("Reproducción iniciada.%s", "\n");
	} else {
		testFAIL("Falló.%s","\n");
		exit(EXIT_FAILURE);
	}

	// Start player sound, using his detached SoundAPI.
	testBEGIN("Iniciando reproducción de los sonidos del player.\n");
	err = coralSoundAPI->play(audioFile[1], true, DEFAULT_UNIT_GAIN);
	if (err == SSerror::SS_NO_ERROR) {
		testSUCCESS("Reproducción iniciada.%s", "\n");
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Start playlist playback
	testBEGIN("Iniciando reproducción de playlist \"%s\".\n", playlist1.c_str());
	ASSERT(SH.existsPlaylist(playlist1));
	err = SH.startPlaylist(playlist1);
	if (err != SSerror::SS_NO_ERROR) {
		testFAIL("Falló, con error %d\n", err);
		exit(EXIT_FAILURE);
	}
	pstate = SH.getPlaylistPlayState(playlist1);
	if (pstate != SSplayback::SS_PLAYING) {
		testFAIL("Falló, playback state obtenido: %d\n", pstate);
		exit(EXIT_FAILURE);
	} else {
		bool playing_something(false);
		SoundManager& sMgr(SoundManager::getInstance());
		for (uint i = START_PLSOUNDS ; i < NUM_SFILES ; i++) {
			playing_something |= sMgr.isPlayingEnvSound(audioFile[i]);
		}
		ASSERT(playing_something);
		testSUCCESS("Reproducción iniciada.\n");
	}

	// Add garbage
	soundsList.clear();
	SH.newPlaylist("lista_inutilizada", soundsList);

	// Start a parallel playlist with different options
	soundsList.push_back(audioFile[4]);
	soundsList.push_back(audioFile[5]);
	Ogre::String fails = SH.newPlaylist(playlist2,
										soundsList,
										true,	// Repeat on end
										false,	// No random order
										2.0f);	// 2 seconds silence
	testBEGIN("Iniciando reproducción de playlist \"%s\".\n", playlist2.c_str());
	if (!fails.empty() || !SH.existsPlaylist(playlist2)) {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}
	err = SH.startPlaylist(playlist2);
	if (err != SSerror::SS_NO_ERROR) {
		testFAIL("Falló, con error %d\n", err);
		exit(EXIT_FAILURE);
	}
	pstate = SH.getPlaylistPlayState(playlist2);
	if (pstate != SSplayback::SS_PLAYING) {
		testFAIL("Falló, playback state obtenido: %d\n", pstate);
		exit(EXIT_FAILURE);
	}
	testSUCCESS("Reproducción iniciada.\n");

	{// Check former playlists is in good shape
		pstate = SH.getPlaylistPlayState(playlist1);
		ASSERT(pstate == SSplayback::SS_PLAYING);
		SoundManager& sMgr(SoundManager::getInstance());
		ASSERT(sMgr.isPlayingEnvSound(audioFile[2])
				|| sMgr.isPlayingEnvSound(audioFile[3]));
	}

	printf("\n\n\33[01;34mSound playback control options:\n\33[22;32m"
			" ¤ \33[01;34mSPACE\33[22;32m   : restart all playing sounds.\n"
			" ¤ \33[01;34mNUMPAD 0\33[22;32m: toogle play/pause  of all sounds.\n"
			" ¤ \33[01;34mNUMPAD 1\33[22;32m: toogle play/pause  of units' sounds.\n"
			" ¤ \33[01;34mNUMPAD 2\33[22;32m: toogle play/pause  of environmental music.\n"
			" ¤ \33[01;34mNUMPAD 3\33[22;32m: toogle fade in/out of all sounds.\n"
			" ¤ \33[01;34mNUMPAD ENTER\33[22;32m: stop all playing sounds.\n"
			"\33[0m\n");

	return;
}


/******************************************************************************/
/*********************    LEVEL AND UNITS LOADING     *************************/


void
Test::createLevelManager(void)
{
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	mLevelManager.load(ent, 25000, 25000, 100, 100);
	mLevelManager.showTriangles();
}


void
Test::createPlayer(void)
{
	GameObject::collMng = mLevelManager.getCollisionManager();
	Ogre::Real	MAX_VEL = 40.0f;
	PlayerSMTTable *tt = PlayerFSMBuilder::build();
	PlayerUnit::setSMTransitionTable(tt);
	Weapon::Ammunition ammo;
	Gun9mm *w(0);

	// Create the player group
	PlayerGroup::setLevelManager(&mLevelManager);
	mPlayerGroup = new PlayerGroup;

	for (int i = 0 ; i < 1 ; i++){
		PlayerUnit *pu(0);
//		Ogre::SceneNode* coralNode(0);	<-- global var
//		Ogre::Entity* coral(0); 		<-- global var

		coral = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
		coralNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		ASSERT(coral);
		ASSERT(coralNode);

		pu = new PlayerUnit;
		pu->setEntity(coral);
		pu->setSceneNode(coralNode);
		pu->setHeight(5);
		pu->build();
		pu->setLife(NUM_LIFE);
		//node->showBoundingBox(true);
		pu->setMaxVelocity(MAX_VEL*2.0f);
		pu->setVelocity(MAX_VEL);
		pu->setPosition(pos1);

		mPlayers.push_back(pu);
		mPlayerGroup->addUnit(pu);
	}

	CircularFormation *formation = new CircularFormation;
	mPlayerGroup->setCreator(mPlayers[0]);
	mPlayerGroup->setFormation(formation);
	mPlayerGroup->build();

	static ShootContainer sc;
	Shoot::setCollisionManager(mLevelManager.getCollisionManager());
	Shoot::setContainer(&sc);
	Shoot::setUpdObjsManager(&mUpdMngr);
	for(int i = 0; i < 10; ++i){
		Shoot *s = new Shoot;
		s->build(Ogre::Math::RangeRandom(8,16));
	}

	// LongWeapon::setShootContainer(&sc);
	w = new Gun9mm;
	w->setID(WP_ID_9MM);
	w->setOwner(mPlayers.back());
	w->setPower(1);
	w->setSqrRange(90000.0);
	w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
	ammo.ammo = NUM_AMMO;
	w->setAmmunition(ammo);
	mPlayers.back()->addNewWeapon(w);
}


void
Test::createZombies(void)
{
	// set the collision system
	Ogre::Real	MAX_VEL = 10.0f;
	ZombieSMTTable *tt = ZombieFSMBuilder::build();
	ZombieUnit::setSMTransitionTable(tt);

	// create the batery for the zombies
	static BillboardBatery bbb;
	bbb.createSet(20, "Billboard/ZombieBlood", 10);
	ZombieUnit::setBillboardBBlood(&bbb);

	static ZombieQueue q;
	ZombieUnit::setQueue(&q);

	std::vector< std::string > zombiematerials;
	zombiematerials.push_back(std::string("zombie01"));
	zombiematerials.push_back(std::string("zombie02"));
	zombiematerials.push_back(std::string("zombie03"));
	zombiematerials.push_back(std::string("zombie04"));
	zombiematerials.push_back(std::string("zombie05"));
	zombiematerials.push_back(std::string("zombie06"));
	zombiematerials.push_back(std::string("zombie07"));
	zombiematerials.push_back(std::string("zombie08"));
	zombiematerials.push_back(std::string("zombie09"));
	zombiematerials.push_back(std::string("zombie10"));

	std::vector< std::string > perrozombiematerials;
	perrozombiematerials.push_back(std::string("perro01"));
	perrozombiematerials.push_back(std::string("perro02"));
	perrozombiematerials.push_back(std::string("perro03"));
	perrozombiematerials.push_back(std::string("perro04"));

	sm::Vector2 p;
	for(int i = 0; i < 150; ++i){

		Ogre::Entity * ent = 0;

		if((i%2)){
			ent = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
			ent->setMaterialName(zombiematerials[std::rand()%zombiematerials.size()]);
		}else{
			ent = GLOBAL_SCN_MNGR->createEntity("perro.mesh");
			ent->setMaterialName(perrozombiematerials[std::rand()%perrozombiematerials.size()]);
		}

		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		ZombieUnit *zu = new ZombieUnit;
		zu->setEntity(ent);
		zu->setSceneNode(node);
		zu->setHeight(5);
		zu->setAttackPower(1);
		zu->build();
		zu->setLife(19);
		//node->showBoundingBox(true);

		zu->setWalkVelocity(MAX_VEL);
		if(i%2)
			zu->setAttackVelocity(MAX_VEL*2.0f);
		else
			zu->setAttackVelocity(MAX_VEL*3.5f);
		zu->setVelocity(MAX_VEL);

		p.x = 1024 + std::rand()%20;//4909;
		p.y = 1024 + std::rand()%20;//4989;
		zu->setPosition(p);
		mZombies.push_back(zu);
	}
}

void Test::createBombs(void)
{
	Bomb::setCollisionManager(mLevelManager.getCollisionManager());
	Bomb::setUpdObjsManager(&mUpdMngr);

	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("granada.mesh");
	mBomb = new ProximityBomb;
	mBomb->setEntity(ent);
	mBomb->hide();
	mBomb->setPower(99);
	mBomb->setRadius(50);
	static_cast<ProximityBomb *>(mBomb)->setActivationTime(5.0f);
	mPlayers.back()->addBomb(mBomb);

}


// create billboardset
void Test::createBillboardSet(void)
{
	mBillboardSet = GLOBAL_SCN_MNGR->createBillboardSet("testbb", 3);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->setDefaultDimensions(50,50);
	mBillboardSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
	mBillboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	mBillboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	mBillboardSet->setMaterialName("Billboard/GreenCircle");
	mBillboardSet->setVisible(true);
	mBillboardSet->setBounds(Ogre::AxisAlignedBox(
			Ogre::Vector3(0,0,0), Ogre::Vector3(25000,25000,25000)),25000*0.5f);

	debug("Number of billboards: %d\n", mBillboardSet->getNumBillboards());
//	mZombies[0]->getSceneNode()->attachObject(mBillboardSet);

	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(mBillboardSet);
//	mBillboardSet->setCullIndividually(true);
}


// Rulo 5/6/2012:
void
Test::createCollectable(void)
{
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("9mm.mesh");

	ShootContainer *sc = new ShootContainer;
	//LongWeapon::setShootContainer(sc);
	Gun9mm *weapon = new Gun9mm;
	Weapon::Ammunition ammo(NUM_AMMO);
	weapon->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
	weapon->setAmmunition(ammo);

	WeaponBackpackItem *item1 = new WeaponBackpackItem;
	item1->setWeapon(weapon);
	item1->setSection(BackpackDef::SECTION_WEAPON);
	item1->loadFromOverlay("BpItem/9mm");
	////////////
	mCollectable = new CollectableObject();
	mCollectable->setEntity(ent);
	mCollectable->setObject((void *)item1);
	mCollectable->setType(COT_BACKPACK_WEAPON);
	mCollectable->setPosition(Ogre::Vector3(1024,0,1024));
	mCollectable->getNode()->showBoundingBox(true);
}



/******************************************************************************/
/*************************    MAIN FUNCTIONS     ******************************/


Test::Test() : mSoundHandler(SoundHandler::getInstance())
{
	Ogre::String fails("");
	std::vector<Ogre::String> sounds;

	// Setup sound system, by creating the sound handler.
	testBEGIN("Revisando la creación del SoundHandler.\n");
	ASSERT(&mSoundHandler == &SoundHandler::getInstance());
	testSUCCESS("SoundManager creado correctamente.\n");

	// Print some info about the available audio devices.
	printDevices();

	// Attach an image to the mouse pointer.
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(),
									 GLOBAL_WINDOW->getHeight());

	// Load sounds into the system.
	// Streaming buffers.
	sounds.push_back(audioFile[0]);
	sounds.push_back(audioFile[1]);
	testBEGIN("Cargando sonidos streaming.\n");
	fails = mSoundHandler.loadStreamSounds(sounds);
	if (fails.empty()) {
		testSUCCESS("%lu sonidos streaming cargados.\n", sounds.size());
	} else {
		testFAIL("Falló la carga de algunos de los archivos:\n%s", fails.c_str());
		exit(EXIT_FAILURE);
	}
	// Loaded buffers.
	sounds.clear();
	sounds.push_back("roar.wav");	// audioFile[2]
	sounds.push_back("fxM2.ogg");	// audioFile[5]
	sounds.push_back("fxZ1.ogg");
	sounds.push_back("fxZ2.ogg");
	sounds.push_back("fxZ3.ogg");
	sounds.push_back("fxZ4.ogg");
	sounds.push_back("fxZ5.ogg");
	sounds.push_back("fxZ6.ogg");
	sounds.push_back("fxZ7.ogg");	// audioFile[3]
	sounds.push_back("fxZ8.ogg");
	sounds.push_back("fxZ9.ogg");	// audioFile[4]
	sounds.push_back("fxZ10.ogg");
	testBEGIN("Cargando sonidos directos (LOADED)\n");
	fails = mSoundHandler.loadDirectSounds(sounds);
	if (fails.empty()) {
		testSUCCESS("%lu sonidos directos cargados.\n", sounds.size());
	} else {
		testFAIL("Falló la carga de algunos de los archivos:\n%s", fails.c_str());
		exit(EXIT_FAILURE);
	}

	// Create some sources to play the sounds.
	testBEGIN("%s","Creando sources para streaming sounds.\n");
	if (mSoundHandler.addStreamSources(NUM_SSOURCES) == SSerror::SS_NO_ERROR) {
		testSUCCESS("%d Streaming Sources creadas.\n", NUM_SSOURCES);
	} else {
		testFAIL("%s","Falló.\n");
		exit(EXIT_FAILURE);
	}
	testBEGIN("%s","Creando sources para direct (loaded) sounds.\n");
	if (mSoundHandler.addDirectSources(NUM_LSOURCES) == SSerror::SS_NO_ERROR) {
		testSUCCESS("%d Loaded Sources creadas.\n", NUM_LSOURCES);
	} else {
		testFAIL("%s","Falló.\n");
		exit(EXIT_FAILURE);
	}

	sounds.clear();
	return;
}


Test::~Test(){}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mMenuManager.build(GLOBAL_WINDOW->getWidth(),GLOBAL_WINDOW->getHeight(),5,5);
	IMenu::setMenuManager(&mMenuManager);
	////////////

	createLevelManager();
	testCollissionRaycast();
	createPlayer();
	createBombs();
	createBillboardSet();
	createCollectable();

	// create a collision object (primera manzana)
	static CollisionObject co;
	co.bb.setSize(1014 - 548, 896-433);
	co.bb.setPosition(sm::Vector2((1014+566)/2.0f , (893+433)/2.0f));
	co.maskFlag = COL_FLAG_UNIT_PLAYER;
	co.userDefined = 0;
	DRAWER.createBox(co.bb);

	// Attach the camera to the SoundManager
	SoundManager::getInstance().setCamera(GLOBAL_CAMERA);

	// Create an external SoundAPI for the player
	testBEGIN("Creando una SoundAPI externa al player\n");
	coralSoundAPI = new SoundAPI(coralNode);
	if (coralSoundAPI) {
		testSUCCESS("SoundAPI creada.\n");
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Create an empty playlist
	Ogre::String name("Vacía");
	std::vector<Ogre::String> soundsList;
	testBEGIN("Creando Playlist \"%s\".\n", name.c_str());
	mSoundHandler.newPlaylist(name, soundsList);
	if (mSoundHandler.existsPlaylist(name)) {
		testSUCCESS("Playlist \"%s\" creada.\n", name.c_str());
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Recreate the same playlist
	testBEGIN("Reinsertando otra Playlist con el mismo nombre.\n");
	Ogre::String fails = mSoundHandler.newPlaylist(name, soundsList);
	if (!fails.empty()) {
		testSUCCESS("Éxito. Mensaje de error recibido: %s", fails.c_str());
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Rename that playlist
	testBEGIN("Renombrando playlist.\n");
	bool renamed = mSoundHandler.renamePlaylist(name, playlist1);
	if (!renamed) {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	} else {
		ASSERT(mSoundHandler.existsPlaylist(playlist1));
		ASSERT(!mSoundHandler.existsPlaylist(name));
		mSoundHandler.renamePlaylist(playlist1, name);
		ASSERT(mSoundHandler.existsPlaylist(name));
		ASSERT(!mSoundHandler.existsPlaylist(playlist1));
		testSUCCESS("Éxito.\n");
	}

	// Delete playlist
	testBEGIN("Borrando playlist \"%s\".\n", name.c_str());
	ASSERT(mSoundHandler.existsPlaylist(name));
	mSoundHandler.deletePlaylist(name);
	if (mSoundHandler.existsPlaylist(name)) {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	} else {
		testSUCCESS("Playlist eliminada correctamente.\n");
	}

	// Create a non-empty playlist
	testBEGIN("Creando Playlist \"%s\".\n", playlist1.c_str());
	soundsList.push_back(audioFile[2]);
	soundsList.push_back("no_existe.mp5");
	soundsList.push_back(audioFile[3]);
	soundsList.push_back("tampoco_existe.ogg");
	fails = mSoundHandler.newPlaylist(playlist1, soundsList);
	if (!fails.empty() && mSoundHandler.existsPlaylist(playlist1)) {
		testSUCCESS("Playlist \"%s\" creada. No se pudieron cargar (como "
				"correspondía) los sonidos:\n%s", playlist1.c_str(), fails.c_str());
		debugGREEN("Sí se pudieron cargar los sonidos:\n");
		soundsList = mSoundHandler.getPlaylistSounds(playlist1);
		for (uint i=0 ; i < soundsList.size() ; i++) {
			debugGREEN(	"%s\n", soundsList[i].c_str());
		}
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Check playlist default creation values
	testBEGIN("Comprobando los valores por defecto de creación de playlists.\n");
	if (mSoundHandler.getPlaylistPlayState(playlist1) != SSplayback::SS_NONE ||
		mSoundHandler.getPlaylistRandomOrder(playlist1)						 ||
		mSoundHandler.getPlaylistRandomSilence(playlist1)					 ||
		!mSoundHandler.getPlaylistRepeat(playlist1)) {
		testSUCCESS("Valores correctos: repeat, !random, !random_silence, "
					"state == SSplayback::NONE\n");
	} else{
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Check playlist values modification
	ASSERT(mSoundHandler.existsPlaylist(playlist1));
	testBEGIN("Modificando valores de reproducción de los playlists.\n");
	mSoundHandler.setPlaylistRandomOrder(playlist1, true);
	ASSERT(mSoundHandler.getPlaylistRandomOrder(playlist1));
	mSoundHandler.setPlaylistRandomSilence(playlist1, true);
	ASSERT(mSoundHandler.getPlaylistRandomSilence(playlist1));
	mSoundHandler.setPlaylistRepeat(playlist1, false);
	mSoundHandler.setPlaylistRepeat(playlist1, true);
	ASSERT(mSoundHandler.getPlaylistRepeat(playlist1));
	testSUCCESS("Valores seleccionados: repeat, random, random_silence.\n");


	// Initialize RNG
	std::srand(time(NULL));
}



/* Function called every frame.
 * Use Common::GlobalObjects::lastTimeFrame */
void Test::update()
{
	int size(0);
	static float counter(0.0f);

	if (counter == 0.0f) {
		// Start environmental and player sounds, and print control options.
		soundInitMisc(mSoundHandler);
		counter += (1E-20);
	}

	counter += GLOBAL_TIME_FRAME;
	mSoundHandler.update();
	handleInput();

	// update the game objects
	size = mZombies.size()-1;
	for(int i=0 ; i <= size ; ++i){
		mZombies[i]->update();
	}
}


// pause all unit sounds
void
Test::pauseUnitsSounds()
{
	SoundManager& sMgr(SoundManager::getInstance());
	bool playing[NUM_SFILES];
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		playing[i] = sMgr.isPlayingEnvSound(audioFile[i]);
	}

	// Pause everything.
	sMgr.globalPause();

	// Environmental sounds which were playing must continue playing.
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		if (playing[i]) {
			sMgr.playEnvSound(audioFile[i], DEFAULT_ENV_GAIN, true);
		}
	}
}


// Play again all paused unit sounds
void
Test::playUnitsSounds()
{
	SoundManager& sMgr(SoundManager::getInstance());
	bool playing[NUM_SFILES];
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		playing[i] = sMgr.isPlayingEnvSound(audioFile[i]);
	}

	// Play everything again
	sMgr.globalPlay();

	// Environmental sounds which were paused must continue paused.
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		if (!playing[i]) {
			sMgr.pauseEnvSound(audioFile[i]);
		}
	}
}


// pause all environmental sounds
void
Test::pauseEnvSounds()
{
	SoundManager& sMgr(SoundManager::getInstance());
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		if (sMgr.isPlayingEnvSound(audioFile[i])) {
			sMgr.pauseEnvSound(audioFile[i]);
		}
	}
}


// play again all paused environmental sounds
void
Test::playEnvSounds()
{
	SoundManager& sMgr(SoundManager::getInstance());
	for (uint i=0 ; i < NUM_SFILES ; i++) {
		if (sMgr.isActiveEnvSound(audioFile[i])) {
			sMgr.playEnvSound(audioFile[i], DEFAULT_ENV_GAIN, true);
		}
	}
}


void
Test::testCollissionRaycast(void)
{
	CollisionObject co;
	float tlx, tly, brx, bry;

	// set the size
	tlx = 10; tly = 100;
	brx = 150; bry = 10;
	co.bb.setSize(brx - tlx, tly-bry);
	co.bb.setPosition(sm::Vector2((tlx+brx)/2.0f,
			(tly+bry)/2.0f));
	co.maskFlag = COL_FLAG_UNIT_PLAYER;
	co.userDefined = 0;
	mLevelManager.getCollisionManager()->addStaticObject(&co);

	// create the lines here and test
	CollisionResult result;
	std::vector<sm::Point> points;
	sm::Point p1,p2;

	p1.x = 9; p1.y = 19;
	p2.x = 20; p2.y = 0;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 140; p1.y = 7;
	p2.x = 220; p2.y = 190;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 9; p1.y = 70;
	p2.x = 220; p2.y = 70;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 100; p1.y = 170;
	p2.x = 130; p2.y = 7;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 5; p1.y = 70;
	p2.x = 9; p2.y = 70;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 0);
}


void Test::testCollectObject(void)
{
	for(int i = mPlayers.size()-1; i>= 0; --i){
		mPlayers[i]->collectObject(mCollectable);
	}
}


void Test::testStart(void)
{
	createZombies();
}


void Test::testEngageEveryone(void){

	GameUnit * target = mPlayers[0];
	for(int i = mZombies.size()-1; i>= 0; --i){
		mZombies[i]->setActualTartet(target);
		mZombies[i]->getFSM().newEvent(ZombieUnit::E_ENGAGE);
	}

}
