/*
 * HUDTest.cpp
 *
 *  Created on: Nov 9, 2013
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
#include <xml/XMLHelper.h>
#include <debug/PrimitiveDrawer.h>

#include "HUDTest.h"



/******************************************************************************/
/********************    GLOBAL VARIABLES AND STUFF    ************************/
namespace {



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
HUDTest::HUDTest() :
    core::AppTester(mTimeFrame),
    mNode(0),
    mEntity(0),
    mOrbitCamera(mCamera, mSceneMgr, mTimeFrame),
    mInputHelper(getMouseButtons(), getKeyboardKeys()),
    mHUD()
{
	// Sound system setup auxiliary variables
	Ogre::String fails("");
	std::vector<Ogre::String> sounds;

    // Configure input systems
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // Configure the mouse cursor
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
    mMouseCursor.updatePosition(mWindow->getWidth() / 2,
                                mWindow->getHeight() / 2);

	// Check initial HUD status is OK
	testBEGIN("Revisando la creación del HUD.\n");
	ASSERT(!mHUD.isVisible());
	testSUCCESS("HUD creado correctamente.\n");

	return;
}



///////////////////////////////////////////////////////////////////////////////
HUDTest::~HUDTest() { /* Auto-generated destructor stub */ }



///////////////////////////////////////////////////////////////////////////////
void
HUDTest::loadAdditionalData(void)
{
    // Ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // Build HUD
	testBEGIN("Construyendo el overlay y los elementos del HUD.\n");
	bool success = mHUD.build();
	if (success) {
		testSUCCESS("Éxito.\n");
	} else {
		testFAIL("Falló.\n");
		exit(EXIT_FAILURE);
	}

	// Setup world floor
	if (!loadFloor()) {
		debugERROR("Errors creating world floor, aborting.\n");
		exit(EXIT_FAILURE);
	}

    // Print out keyboard controls
    printf("\n\n\33[01;34mHUD control options:\n\33[22;32m"
			" ¤ \33[01;34mNUMPAD 1\33[22;32m: set weapon 9mm.\n"
			" ¤ \33[01;34mNUMPAD 2\33[22;32m: set weapon fal.\n"
			"\33[0m\nLast command:\n");
}



///////////////////////////////////////////////////////////////////////////////
void
HUDTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update the camera
    handleCameraInput();

    // update the keyboard-triggered HUD events
    handlePlayerInput();
}



////////////////////////////////////////////////////////////////////////////////
bool
HUDTest::parseXML(const std::string& xmlFName, std::string& meshName) const
{
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <HUDTest mesh="file.mesh" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "HUDTest") != 0) {
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
HUDTest::handleCameraInput()
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
HUDTest::handlePlayerInput(void)
{
	static bool keyPressed(false);

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_NUMPAD1)) {
    	if (!keyPressed) {
    		keyPressed = true;
			fprintf(stderr, "\r");
			debugBLUE("Switchig weapon to \"9mm\"    ");
			mHUD.updateWeapon(cz::WID_9MM);
    	}

    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_NUMPAD2)) {
		if (!keyPressed) {
			keyPressed = true;
			fprintf(stderr, "\r");
			debugBLUE("Switchig weapon to \"fal\"    ");
			mHUD.updateWeapon(cz::WID_9MM);
		}

    } else if (keyPressed) {
    	keyPressed = false;
    }

	return;
}



///////////////////////////////////////////////////////////////////////////////
bool
HUDTest::loadFloor(void)
{
    Ogre::Plane p(0.0f, 0.0f, 1.0f, 0.0f);  // normal:(0,0,1) ; distance:0
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);

    // Create entity for the floor
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    if(!ent) {
    	debugERROR("Couldn't create entity for the floor object.\n");
    	return false;
    } else {
		ent->setMaterialName("GrassSample");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    }

    return true;
}


}
