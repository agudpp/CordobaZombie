/*
 * SoundTest.cpp
 *
 *  Created on: September 24, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include <map>
#include <memory>
#include <cstring>

#include <OgreMath.h>
#include <OgreCamera.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreString.h>

#include <tinyxml/tinyxml.h>
#include <debug/PrimitiveDrawer.h>
#include <xml/XMLHelper.h>

#include "SoundTest.h"
#include "sound/SoundEnums.h"



/******************************************************************************/
/********************    GLOBAL VARIABLES AND STUFF    ************************/
namespace {


// Number of buffers & sources for direct play
//
#define  NUM_LBUFFERS  20
#define  NUM_LSOURCES  60


// Number of buffers & sources for streaming
//
#define  NUM_SBUFFERS  5
#define  NUM_SSOURCES  NUM_SBUFFERS


// Playlists
//
const Ogre::String playlist1("lista1");

}



/******************************************************************************/
/**********************    LOCAL HELPER FUNCTIONS    **************************/
namespace {

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
    buttons.reserve((1<<6));

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

    return buttons;
}


}



/******************************************************************************/
/***********************    CLASS IMPLEMENTATION    ***************************/
namespace tests {

///////////////////////////////////////////////////////////////////////////////
SoundTest::SoundTest() :
    core::AppTester(mTimeFrame),
    mNode(0),
    mEntity(0),
    mOrbitCamera(mCamera, mSceneMgr, mTimeFrame),
    mInputHelper(getMouseButtons(), getKeyboardKeys()),
	mSH(mm::SoundHandler::getInstance())
{
	// Sound system setup auxiliary variables
	Ogre::String fails("");
	std::vector<Ogre::String> sounds;

    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

	// Check initial sound system status is OK
	testBEGIN("Revisando la creación del SoundHandler.\n");
	ASSERT(&mSH == &mm::SoundHandler::getInstance());
	testSUCCESS("SoundHandler creado correctamente.\n");

	// Print some info about the available audio devices.
	printDevices();

	// Load sounds into the system.
	// Streaming buffers.
	sounds.push_back("fxA20.ogg");
	sounds.push_back("Siren.ogg");
	testBEGIN("Cargando sonidos streaming.\n");
	fails = mSH.loadStreamSounds(sounds);
	if (fails.empty()) {
		testSUCCESS("%lu sonidos streaming cargados.\n", sounds.size());
	} else {
		testFAIL("Falló la carga de algunos de los archivos:\n%s", fails.c_str());
		exit(EXIT_FAILURE);
	}
	// Loaded buffers.
	sounds.clear();
	sounds.push_back("roar.wav");
	sounds.push_back("fxM2.ogg");
	sounds.push_back("fxZ1.ogg");
	sounds.push_back("fxZ2.ogg");
	sounds.push_back("fxZ3.ogg");
	sounds.push_back("fxZ4.ogg");
	sounds.push_back("fxZ5.ogg");
	sounds.push_back("fxZ6.ogg");
	sounds.push_back("fxZ7.ogg");
	sounds.push_back("fxZ8.ogg");
	sounds.push_back("fxZ9.ogg");
	sounds.push_back("fxZ10.ogg");
	testBEGIN("Cargando sonidos directos (LOADED)\n");
	fails = mSH.loadDirectSounds(sounds);
	if (fails.empty()) {
		testSUCCESS("%lu sonidos directos cargados.\n", sounds.size());
	} else {
		testFAIL("Falló la carga de algunos de los archivos:\n%s", fails.c_str());
		exit(EXIT_FAILURE);
	}

	// Create sound sources to play the sounds.
	testBEGIN("%s","Creando sources para streaming sounds.\n");
	if (mSH.addStreamSources(NUM_SSOURCES) == mm::SSerror::SS_NO_ERROR) {
		testSUCCESS("%d Streaming Sources creadas.\n", NUM_SSOURCES);
	} else {
		testFAIL("%s","Falló.\n");
		exit(EXIT_FAILURE);
	}
	testBEGIN("%s","Creando sources para direct (loaded) sounds.\n");
	if (mSH.addDirectSources(NUM_LSOURCES) == mm::SSerror::SS_NO_ERROR) {
		testSUCCESS("%d Loaded Sources creadas.\n", NUM_LSOURCES);
	} else {
		testFAIL("%s","Falló.\n");
		exit(EXIT_FAILURE);
	}

	return;
}



///////////////////////////////////////////////////////////////////////////////
SoundTest::~SoundTest()
{
    // TODO Auto-generated destructor stub
}



///////////////////////////////////////////////////////////////////////////////
void
SoundTest::loadAditionalData(void)
{
    // Ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

	// Attach the camera to the listener of the sound system
	mSH.setCamera(mCamera);

	// Playlists functionality checking
	testPlaylists();
}



///////////////////////////////////////////////////////////////////////////////
void
SoundTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
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



////////////////////////////////////////////////////////////////////////////////
bool
SoundTest::parseXML(const std::string& xmlFName, std::string& meshName) const
{
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <SoundTest mesh="file.mesh" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "SoundTest") != 0) {
        debugERROR("Invalid xml root element name\n");
        return false;
    }

    // check now for the value we want
    if (root->Attribute("mesh") == 0) {
        debugERROR("mesh attribute not found in the xml file... invalid xml\n");
        return false;
    }

    // everything fine
    meshName = root->Attribute("mesh");
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

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN) ||
        mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mOrbitCamera.moveCamera(mTranslationVec);
    }
    if(zoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(zoom);
    }

    const float lMouseZ = input::Mouse::relZ();
    float scrollZoom = mOrbitCamera.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if(scrollZoom != mOrbitCamera.zoom()) {
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
	static bool keyPressed(false);
	static std::map<input::KeyCode,mm::SSplayback> playingState =
			{{input::KeyCode::KC_NUMPAD0, mm::SSplayback::SS_PLAYING}};

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_NUMPAD0)) {
    	if (!keyPressed) {
    		keyPressed = true;
			if (playingState[input::KeyCode::KC_NUMPAD0]
							 == mm::SSplayback::SS_PLAYING) {
				mSH.globalPause();
				playingState[input::KeyCode::KC_NUMPAD0] = mm::SSplayback::SS_PAUSED;
				debugBLUE("Global sounds PAUSED.%s", "\n");
			} else {
				mSH.globalPlay();
				playingState[input::KeyCode::KC_NUMPAD0] = mm::SSplayback::SS_PLAYING;
				debugBLUE("Global sounds PLAY.%s", "\n");
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
	for (unsigned int i=0 ; i < soundDevices.size() ; i++) {
		std::cout << "  ¤ " << soundDevices[i] << std::endl;
	}
	std::cout << "Using sound device:" << mSH.getSoundDevice() << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
void
SoundTest::testPlaylists(void)
{
	// Create an empty playlist
	Ogre::String name("Vacía");
	std::vector<Ogre::String> soundsList;
	testBEGIN("Creando Playlist \"%s\".\n", name.c_str());
	mSH.newPlaylist(name, soundsList);
	if (mSH.existsPlaylist(name)) {
		testSUCCESS("Playlist \"%s\" creada.\n", name.c_str());
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Recreate the same playlist
	testBEGIN("Reinsertando otra Playlist con el mismo nombre.\n");
	Ogre::String fails = mSH.newPlaylist(name, soundsList);
	if (!fails.empty()) {
		testSUCCESS("Éxito. Mensaje de error recibido: %s", fails.c_str());
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Rename that playlist
	testBEGIN("Renombrando playlist.\n");
	bool renamed = mSH.renamePlaylist(name, playlist1);
	if (!renamed) {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	} else {
		ASSERT(mSH.existsPlaylist(playlist1));
		ASSERT(!mSH.existsPlaylist(name));
		mSH.renamePlaylist(playlist1, name);
		ASSERT(mSH.existsPlaylist(name));
		ASSERT(!mSH.existsPlaylist(playlist1));
		testSUCCESS("Éxito.\n");
	}

	// Delete playlist
	testBEGIN("Borrando playlist \"%s\".\n", name.c_str());
	ASSERT(mSH.existsPlaylist(name));
	mSH.deletePlaylist(name);
	if (mSH.existsPlaylist(name)) {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	} else {
		testSUCCESS("Playlist eliminada correctamente.\n");
	}

	// Create a non-empty playlist
	testBEGIN("Creando Playlist \"%s\".\n", playlist1.c_str());
	soundsList.push_back("roar.wav");
	soundsList.push_back("no_existe.mp5");
	soundsList.push_back("fxZ7.ogg");
	soundsList.push_back("tampoco_existe.ogg");
	fails = mSH.newPlaylist(playlist1, soundsList);
	if (!fails.empty() && mSH.existsPlaylist(playlist1)) {
		testSUCCESS("Playlist \"%s\" creada. No se pudieron cargar (como "
				"correspondía) los sonidos:\n%s", playlist1.c_str(), fails.c_str());
		debugGREEN("Sí se pudieron cargar los sonidos:\n");
		soundsList = mSH.getPlaylistSounds(playlist1);
		for (uint i=0 ; i < soundsList.size() ; i++) {
			debugGREEN(	"%s\n", soundsList[i].c_str());
		}
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Check playlist default creation values
	testBEGIN("Comprobando los valores por defecto de creación de playlists.\n");
	if (mSH.getPlaylistPlayState(playlist1) != mm::SSplayback::SS_NONE	||
		mSH.getPlaylistRandomOrder(playlist1)							||
		mSH.getPlaylistRandomSilence(playlist1)							||
		!mSH.getPlaylistRepeat(playlist1)) {
		testSUCCESS("Valores correctos: repeat, !random, !random_silence, "
					"state == SSplayback::NONE\n");
	} else{
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Check playlist values modification
	ASSERT(mSH.existsPlaylist(playlist1));
	testBEGIN("Modificando valores de reproducción de los playlists.\n");
	mSH.setPlaylistRandomOrder(playlist1, true);
	ASSERT(mSH.getPlaylistRandomOrder(playlist1));
	mSH.setPlaylistRandomSilence(playlist1, true);
	ASSERT(mSH.getPlaylistRandomSilence(playlist1));
	mSH.setPlaylistRepeat(playlist1, false);
	mSH.setPlaylistRepeat(playlist1, true);
	ASSERT(mSH.getPlaylistRepeat(playlist1));
	testSUCCESS("Valores seleccionados: repeat, random, random_silence.\n");

	return;
}


}
