/*
 * SoundTest.cpp
 *
 *  Created on: September 24, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include <map>
#include <vector>
#include <memory>
#include <ctime>  // time()

#include <OgrePrerequisites.h>
#include <OgreMath.h>
#include <OgreString.h>
#include <OgreCamera.h>
#include <OgrePlane.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreMeshManager.h>
#include <OgreFontManager.h>
#include <OgreResourceManager.h>

#include <tinyxml/tinyxml.h>
#include <debug/DebugUtil.h>
#include <debug/PrimitiveDrawer.h>
#include <ResourceHandler.h>
#include <ResourceGroup.h>

#include "SoundTest.h"
#include <sound/SoundEnums.h>
#include <sound/SoundAPI.h>



/******************************************************************************/
/********************    GLOBAL VARIABLES AND STUFF    ************************/
namespace {

// Number of sources for direct play
//
#define  NUM_LSOURCES  25

// Number of sources for streaming
//
#define  NUM_SSOURCES  4

// Audio files
//
#define  NUM_SFILES      7
#define  START_PLSOUNDS  2
const char *audioFile[NUM_SFILES] = {
    "fxA20.ogg",
    "Siren.ogg",
    "fxM2.ogg", // playlist 0 sound 0
    "fxZ7.ogg", // playlist 0 sound 1
    "fxZ9.ogg", // playlist 1 sound 0
    "fxZ6.ogg", // playlist 1 sound 1
    "fxZ5.ogg"  // playlist 1 sound 2
};

// Punctual (e.g. not environmental) sound playback
//
core::Primitive* sirenSphere(0);
mm::SoundAPI* sirenSoundAPI(0);

// Playlists
//
#define  NUM_PLAYLISTS  4
const Ogre::String playlist[NUM_PLAYLISTS] = {
    "lista1",
    "lista2",
    "dummy",
    "water"
};

// Audio fading (in-out) times
//
#define  FADE_TIME  2.5f


/******************************************************************************/
/**********************    LOCAL HELPER FUNCTIONS    **************************/

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);

    return buttons;
}

// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve((1 << 6));

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_A);
    buttons.push_back(input::KeyCode::KC_S);
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_W);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_DOWN);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_UP);
    buttons.push_back(input::KeyCode::KC_1);
    buttons.push_back(input::KeyCode::KC_2);
    buttons.push_back(input::KeyCode::KC_E);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_ADD);
    buttons.push_back(input::KeyCode::KC_MINUS);
    buttons.push_back(input::KeyCode::KC_NUMPAD0);
    buttons.push_back(input::KeyCode::KC_NUMPAD1);
    buttons.push_back(input::KeyCode::KC_NUMPAD2);
    buttons.push_back(input::KeyCode::KC_NUMPAD3);
    buttons.push_back(input::KeyCode::KC_SPACE);

    return buttons;
}

}


/******************************************************************************/
/***********************    CLASS IMPLEMENTATION    ***************************/
namespace tests {


const std::string SoundTest::SOUNDTEST_RC_FILES[SOUNDTEST_NUM_RC_FILES] = {
    "multimedia/sonido/resources.cfg",
    "multimedia/musica/resources.cfg"
};


///////////////////////////////////////////////////////////////////////////////
SoundTest::SoundTest() :
    core::AppTester(mTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mRH(0)
,   mOpenALHandler(true)
,   mSH(mm::SoundHandler::getInstance())
{
    // Setup resources system
    testBEGIN("Indexando archivos de audio del sistema.\n");
    if (!loadResources()) {
        testFAIL("Falló la carga de recursos.\n");
        exit(EXIT_FAILURE);
    }
    testSUCCESS("%d archivos de recursos indexados con éxito.\n",
                SOUNDTEST_NUM_RC_FILES);

    // Configure input systems
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // Check initial sound system status is OK
    testBEGIN("Revisando la creación del SoundHandler.\n");
    mSH.soundManager()->setOpenALHandler(&mOpenALHandler);
    ASSERT(&mSH == &mm::SoundHandler::getInstance());
    printDevices();
    testSUCCESS("SoundHandler creado correctamente.\n");

    // Load sounds into the system.
    // Streaming buffers.
    std::vector<Ogre::String> sounds;
    sounds.push_back("fxA20.ogg"); // "water sound"
    sounds.push_back("Siren.ogg");
    sounds.push_back("Siren.wav");
    testBEGIN("Cargando sonidos streaming.\n");
    Ogre::String fails = mSH.loadStreamSounds(*mRH, sounds);
    if (fails.empty()) {
        testSUCCESS("%lu sonidos streaming cargados.\n", sounds.size());
    } else {
        testFAIL("Falló la carga de algunos de los archivos:\n%s",
                 fails.c_str());
        exit(EXIT_FAILURE);
    }
    // Direct (loaded) buffers.
    sounds.clear();
    sounds.push_back("roar.wav");
    sounds.push_back("fxM2.ogg"); // "button pressed"
    sounds.push_back("fxZ1.ogg");
    sounds.push_back("fxZ2.ogg");
    sounds.push_back("fxZ3.ogg");
    sounds.push_back("fxZ4.ogg");
    sounds.push_back("fxZ5.ogg");
    sounds.push_back("fxZ6.ogg");
    sounds.push_back("fxZ7.ogg"); // "ferneeeee" variant 2
    sounds.push_back("fxZ8.ogg");
    sounds.push_back("fxZ9.ogg"); // "ole"
    sounds.push_back("fxZ10.ogg");
    testBEGIN("Cargando sonidos directos (loaded)\n");
    fails = mSH.loadDirectSounds(*mRH, sounds);
    if (fails.empty()) {
        testSUCCESS("%lu sonidos directos cargados.\n", sounds.size());
    } else {
        testFAIL("Falló la carga de algunos de los archivos:\n%s",
                 fails.c_str());
        exit(EXIT_FAILURE);
    }

    // Create sound sources to play the sounds.
    testBEGIN("%s", "Creando sources para streaming sounds.\n");
    if (mSH.addStreamSources(NUM_SSOURCES) == mm::SSerror::SS_NO_ERROR) {
        testSUCCESS("%d Streaming Sources creadas.\n", NUM_SSOURCES);
    } else {
        testFAIL("%s", "Falló.\n");
        exit(EXIT_FAILURE);
    }
    testBEGIN("%s", "Creando sources para direct (loaded) sounds.\n");
    if (mSH.addDirectSources(NUM_LSOURCES) == mm::SSerror::SS_NO_ERROR) {
        testSUCCESS("%d Loaded Sources creadas.\n", NUM_LSOURCES);
    } else {
        testFAIL("%s", "Falló.\n");
        exit(EXIT_FAILURE);
    }

    return;
}


///////////////////////////////////////////////////////////////////////////////
SoundTest::~SoundTest()
{
    // Just kill the SoundHandler, the OpenALHandler takes care of itself
    mSH.shutDown();
    delete mRH;
}


///////////////////////////////////////////////////////////////////////////////
bool
SoundTest::loadResources()
{
    TiXmlDocument cfgFile;
    const TiXmlElement* xmlElem(0);
    const char* rscCfgFile("EngineConfig.xml");
    const char* rscCfgElem("ResourcesSystemLoader");
    const char* rscCfgAttr("rootRscPath");

    // Initialize resources handler
    if (!cfgFile.LoadFile(rscCfgFile)) {
        debugERROR("Could not load local configuration file \"%s\"."
            "Does such file exist in the CWD?\n", rscCfgFile);
        return false;
    }
    xmlElem = cfgFile.RootElement();
    if (!xmlElem) {
        debugERROR("Could not get root element from local configuration file "
            "\"%s\". Does such file exist in the CWD?\n", rscCfgFile);
        return false;
    }
    xmlElem = xmlElem->FirstChildElement(rscCfgElem);
    if (!xmlElem) {
        debugERROR("Could not get \"%s\" element from local configuration file "
            "\"%s\".\n", rscCfgElem, rscCfgFile);
        return false;
    }
    const char* localResourceRootPath = xmlElem->Attribute(rscCfgAttr);
    if (!localResourceRootPath) {
        debugERROR("Could not get any attribute named \"%s\" from element"
            "\"%s\" in local configuration file \"%s\".\n",
            rscCfgAttr, rscCfgElem, rscCfgFile);
        return false;
    }
    debug("Root path for .cfg resources files: %s\n", localResourceRootPath);
    mRH = new rrh::ResourceHandler(localResourceRootPath);
    ASSERT(mRH);

    // Load resources files
    for (unsigned int i = 0 ; i < SOUNDTEST_NUM_RC_FILES ; i++) {
        std::string rscFullPath("");
        bool ok = mRH->getResourceFullPath(SOUNDTEST_RC_FILES[i], rscFullPath);
        if (!ok) {
            debugERROR("Could not load resources file \"%s\"\n",
                     SOUNDTEST_RC_FILES[i].c_str());
            return false;
        }
        rrh::ResourceGroup rg(rscFullPath);
        ok = mRH->loadResourceGroup(rg);
        if (!ok) {
            debugERROR("Could not load resources file \"%s\"\n",
                       rscFullPath.c_str());
            return false;
        }
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
void
SoundTest::loadAditionalData(void)
{
    // Ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) {
        iter.getNext()->load();
    }

    // Attach the camera to the listener of the sound system
    mSH.setCamera(mCamera);

    // Setup world floor
    if (!loadFloor()) {
        debugERROR("Errors creating world floor. ABORTING.\n");
        exit(EXIT_FAILURE);
    }

    // Try to play some sounds
    if (!initSoundsPlayback()) {
        debugERROR("Errors found when trying to play some sounds. ABORTING.\n");
        exit(EXIT_FAILURE);
    } else {
        // Print out keyboard sound controls
        printf("\n\n\33[01;34mSound playback control options:\n\33[22;32m"
               " ¤ \33[01;34mSPACE\33[22;32m   : restart all playing sounds.\n"
               " ¤ \33[01;34mNUMPAD 0\33[22;32m: toogle play/pause  of all sounds.\n"
               " ¤ \33[01;34mNUMPAD 1\33[22;32m: toogle fade in/out of water sound. (FIXME!)\n"
               "\33[0m\nLast command:\n");
    }
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
SoundTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        fprintf(stderr, "\n");
        mStopRunning = true;
        return;
    }

    // update the sound system
    mSH.update(mTimeFrame);

    // update the camera
    handleCameraInput();

    // update the keyboard-triggered sound events
    handleSoundInput();
}


///////////////////////////////////////////////////////////////////////////////
bool
SoundTest::initSoundsPlayback(void)
{
    mm::SSerror err(mm::SSerror::SS_INTERNAL_ERROR);
    mm::SSplayback pstate(mm::SSplayback::SS_NONE);

    Ogre::Vector3 sirenLocation(0.0f, 0.0f, 1.5f);
    Ogre::Real sirenRadius(5.0f);

    Ogre::String fails("");
    std::vector<Ogre::String> soundsList;

    // Play environmental water sound  ////////////////////////////////////////
    //
    testBEGIN("Iniciando reproducción del sonido ambiente fxA20.ogg\n");
    err = mm::SoundManager::getInstance().playEnvSound("fxA20.ogg",
                                                       DEFAULT_ENV_GAIN,
                                                       true);
    if (err == mm::SSerror::SS_NO_ERROR) {
        ASSERT(mm::SoundManager::getInstance().isActiveEnvSound("fxA20.ogg"));
        ASSERT(mm::SoundManager::getInstance().isPlayingEnvSound("fxA20.ogg"));
        testSUCCESS("Reproducción iniciada exitosamente. Eliminando sonido...\n");
        err = mm::SoundManager::getInstance().stopEnvSound("fxA20.ogg");
        ASSERT(err == mm::SSerror::SS_NO_ERROR);
    } else {
        testFAIL("Falló.%s", "\n");
        return false;
    }

    // Setup punctual sound  //////////////////////////////////////////////////
    // FIXME: direct use of SoundAPI, DON'T DO THIS OUTSIDE DEBUG TESTS!
    //
    testBEGIN("Creando SoundAPI puntual (externa a todo player)\n");
    ASSERT(!sirenSphere);
    sirenSphere =
        core::PrimitiveDrawer::instance().createSphere(sirenLocation,
                                                       sirenRadius,
                                                       Ogre::ColourValue::Red);
    ASSERT(sirenSphere);
    sirenSoundAPI = new mm::SoundAPI(sirenSphere->node);
    if (sirenSoundAPI) {
        testSUCCESS("SoundAPI creada.\n");
    } else {
        testFAIL("Falló.\n");
        return false;
    }

    // Play punctual sound, using his (detached) SoundAPI  ////////////////////
    //
	testBEGIN("Iniciando reproducción del sonido puntual %s.\n", audioFile[1]);
	err = sirenSoundAPI->play(audioFile[1], true, DEFAULT_UNIT_GAIN);
	if (err == mm::SSerror::SS_NO_ERROR) {
		testSUCCESS("Reproducción iniciada.%s", "\n");
	} else {
		testFAIL("Falló.\n");
		return false;
	}

    // Create three different playlists  //////////////////////////////////////
    //
    testBEGIN("Creando playlists.\n");
    soundsList.push_back(audioFile[2]);
    soundsList.push_back(audioFile[3]);
    fails = mSH.newPlaylist(playlist[0], soundsList, true, // Repeat on end
                            false, // No random order
                            -1.0f); // Random silence
    if (!fails.empty()) {
        testFAIL("Falló.\n");
        return false;
    }
    soundsList.clear();
    soundsList.push_back(audioFile[4]);
    soundsList.push_back(audioFile[5]);
    soundsList.push_back(audioFile[6]);
    fails = mSH.newPlaylist(playlist[1], soundsList, true, // Repeat on end
                            true, // Random order
                            2.0f); // 2 seconds silence
    if (!fails.empty()) {
        testFAIL("Falló.\n");
        return false;
    }
    soundsList.clear();
    fails = mSH.newPlaylist(playlist[2], soundsList); // Add garbage
    if (!fails.empty()) {
        testFAIL("Falló.\n");
        return false;
    } else if (!mSH.existsPlaylist(playlist[0])
        || !mSH.existsPlaylist(playlist[1])
        || !mSH.existsPlaylist(playlist[2])) {
        testFAIL("Falló.\n");
        return false;
    }
    testSUCCESS("Playlists creadas.\n");

    // Start playlists playback  //////////////////////////////////////////////
    //
	testBEGIN("Iniciando reproducción de playlists.\n");
	for (int i=0 ; i < NUM_PLAYLISTS-1 ; i++) {
		err = mSH.startPlaylist(playlist[i]);
		if (err != mm::SSerror::SS_NO_ERROR) {
			testFAIL("Falló la reproducción del playlist[%d]. Error: %s\n",
					i, SSenumStr(err));
			return false;
		} else {
			ASSERT(mSH.existsPlaylist(playlist[i]));
		}
	}
	testSUCCESS("Reproducción iniciada.\n");

    // Delete playlist  ///////////////////////////////////////////////////////
    //
    testBEGIN("Destruyendo playlists.\n");
    if (mSH.getPlaylistPlayState(playlist[2]) != mm::SSplayback::SS_FINISHED) {
        testFAIL("Falló.\n");
        return false;
    }
    mSH.deletePlaylist(playlist[2]);
    if (mSH.existsPlaylist(playlist[2])) {
        testFAIL("Falló.\n");
        return false;
    }
    testSUCCESS("Playlist \"%s\" destruida.\n", playlist[2].c_str());

    // Embed loose environmental sound into playlist   ////////////////////////
    //
    // NOTE:
    //   This is the correct way of playing lists of environmental sounds.
    //   However, for single looping sounds (like this continuous water sound)
    //   better results are obtained using the SoundManager directly, e.g.
    //   mm::SoundManager::getInstance().playEnvSound(soundName, gain, true);
    //
    testBEGIN("Iniciando el sonido ambiente \"%s\" dentro de un playlist.\n",
              audioFile[0]);
    soundsList.clear();
    soundsList.push_back(audioFile[0]);
    fails = mSH.newPlaylist(playlist[3], soundsList);
    if (!fails.empty()) {
        testFAIL("Falló la creación del nuevo playlist.\n");
        return false;
    }
    ASSERT(mSH.existsPlaylist(playlist[3]));
    err = mSH.startPlaylist(playlist[3], 0.5);
    if (err == mm::SSerror::SS_NO_ERROR) {
        testSUCCESS("Playlist \"%s\" creado e iniciado con éxito.\n",
                    playlist[2].c_str());
    } else {
        testFAIL("Falló la reproducción del nuevo playlist: %s.\n",
                 SSenumStr(err));
        return false;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
void
SoundTest::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.

    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)
        || mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)
        || mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_UP)
        || mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)
        || mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if (mTranslationVec != Ogre::Vector3::ZERO) {
        mOrbitCamera.moveCamera(mTranslationVec);
    }
    if (zoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(zoom);
    }

    const float lMouseZ = input::Mouse::relZ();
    float scrollZoom = mOrbitCamera.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if (scrollZoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(input::Mouse::relX());
    const float lMouseY = float(input::Mouse::relY());
    if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
        const float factor = -0.01 * 1.5f;
        mOrbitCamera.rotateCamera(Ogre::Radian(lMouseX * factor),
                                  Ogre::Radian(lMouseY * factor));
    }

    // check for the type of camera we want to use
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_1)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
    }

    return;
}


///////////////////////////////////////////////////////////////////////////////
void
SoundTest::handleSoundInput(void)
{
    mm::SSerror err(mm::SSerror::SS_NO_ERROR);
    static bool keyPressed(false);
    static std::map<input::KeyCode, mm::SSplayback> playingState =
        {
            { input::KeyCode::KC_NUMPAD0, mm::SSplayback::SS_PLAYING },
            { input::KeyCode::KC_NUMPAD1, mm::SSplayback::SS_FADING_IN } };

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_SPACE)) {
        if (!keyPressed) {
            keyPressed = true;
            mSH.globalRestart();
            fprintf(stderr, "\r");
            debugBLUE("Global sounds RESTART.       ");
        }

    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_NUMPAD0)) {
        if (!keyPressed) {
            keyPressed = true;
            if (playingState[input::KeyCode::KC_NUMPAD0]
                == mm::SSplayback::SS_PLAYING) {
                mSH.globalPause();
                fprintf(stderr, "\r");
                debugBLUE("Global sounds PAUSED.       ");
                playingState[input::KeyCode::KC_NUMPAD0] =
                    mm::SSplayback::SS_PAUSED;
            } else {
                mSH.globalPlay();
                fprintf(stderr, "\r");
                debugBLUE("Global sounds PLAY.       ");
                playingState[input::KeyCode::KC_NUMPAD0] =
                    mm::SSplayback::SS_PLAYING;
            }
        }

    } else if (mInputHelper.isKeyPressed((input::KeyCode::KC_NUMPAD1))) {
        if (!keyPressed) {
            keyPressed = true;
            if (playingState[input::KeyCode::KC_NUMPAD1]
                == mm::SSplayback::SS_FADING_IN) {
                err = mSH.fadeOutPlaylist(playlist[2], 1.5f);
                if (err != mm::SSerror::SS_NO_ERROR) {
                    debugWARNING("\rWater sound FADE OUT failed: %s.",
                                 SSenumStr(err));
                }
                fprintf(stderr, "\r");
                debugBLUE("Water sound FADING OUT.       ");
                playingState[input::KeyCode::KC_NUMPAD1] =
                    mm::SSplayback::SS_FADING_OUT;
            } else {
                err = mSH.fadeInPlaylist(playlist[2], 1.5f);
                if (err != mm::SSerror::SS_NO_ERROR) {
                    debugWARNING("\rWater sound FADE IN failed: %s.",
                                 SSenumStr(err));
                }
                fprintf(stderr, "\r");
                debugBLUE("Water sound FADING IN.       ");
                playingState[input::KeyCode::KC_NUMPAD1] =
                    mm::SSplayback::SS_FADING_IN;
            }
        }

    } else if (keyPressed) {
        keyPressed = false;
    }

    return;
}


///////////////////////////////////////////////////////////////////////////////
void
SoundTest::printDevices(void)
{
    std::cout << "Available sound devices:" << std::endl;
    std::vector<std::string> soundDevices = mSH.getAvailableSoundDevices();
    for (unsigned int i = 0; i < soundDevices.size(); i++) {
        std::cout << "  ¤ " << soundDevices[i] << std::endl;
    }
    std::cout << "Using sound device: " << mSH.getSoundDevice() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
bool
SoundTest::loadFloor(void)
{
    Ogre::Plane p(0.0f, 0.0f, 1.0f, 1.0f); // normal:(0,0,1) ; distance:1
    Ogre::MeshManager::getSingleton().createPlane(
        "FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        p,
        200000,
        200000,
        20,
        20,
        true,
        1,
        9000,
        9000,
        Ogre::Vector3::UNIT_Y);

    // Create entity for the floor
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    if (!ent) {
        debugERROR("Couldn't create entity for the floor object.\n");
        return false;
    } else {
        ent->setMaterialName("GrassSample");
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    }

    return true;
}

}
