/*
 * AppTester.cpp
 *
 *  Created on: 23/12/2010
 *      Author: agustin
 */

#include "AppTester.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <OgreWindowEventUtilities.h>
#include <OgreFontManager.h>

#include <types/basics.h>
#include <os_utils/OSHelper.h>


namespace core {

/******************************************************************************/
void
AppTester::handleKeyboardInputCamera(void)
{
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
    }

    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    const float lCoeff = 800.0f * mGlobalTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.

    if (mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W)) {
        mTranslationVec.z -= lCoeff;
    }
    if (mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S)) {
        mTranslationVec.z += lCoeff;
    }
    if (mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A)) {
        mTranslationVec.x -= lCoeff;
    }
    if (mKeyboard->isKeyDown(OIS::KC_RIGHT)
        || mKeyboard->isKeyDown(OIS::KC_D)) {
        mTranslationVec.x += lCoeff;
    }

    if (mTranslationVec != Ogre::Vector3::ZERO) {
        mBodySceneNode->translate(mTranslationVec, Ogre::Node::TS_LOCAL);
    }

    ///////////////////////////////////////////////////////////////////////////

}

/******************************************************************************/
void
AppTester::handleMouseInputCamera()
{

    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    // now we actualize the direction of the camera.
    const OIS::MouseState& lMouseState = mMouse->getMouseState();
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
    //float lRotCoeff = -5.0f;
    Ogre::Radian lAngleX(lMouseX * -0.01f);
    Ogre::Radian lAngleY(lMouseY * -0.01f);
    // If the 'player' don't make loopings, 'yaw in world' + 'pitch in local' is often enough for a camera controler.
    mBodySceneNode->yaw(lAngleX, Ogre::Node::TS_WORLD);
//	mCameraScnNode->pitch(lAngleY);
    mBodySceneNode->pitch(lAngleY);

    ///////////////////////////////////////////////////////////////////////////

}

/******************************************************************************/
bool
AppTester::loadResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(RESOURCES_CONF_FILE);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

    return true;
}

/******************************************************************************/
bool
AppTester::loadInitialConfig(bool showConfig)
{

    mRoot = new Ogre::Root(PLUGINS_CONF_FILE);

    if (!mRoot) {
        debugERROR("Error creating Ogre::Root\n");
        return false;
    }
    debug("Created the Ogre::Root\n");

    if (!showConfig) {
        // we will try to load everything from the .cfg file directly
        if (!mRoot->restoreConfig()) {
            debugERROR("Error restoring the configuration file, probably not "
                "ogre.cfg is in the binary folder\n");
            return false;
        }
    } else {
        // configure using the window
        if (!mRoot->showConfigDialog()) {
            // The user cancelled the configuration... return then
            debugERROR("The user cancelled the configuration dialog!\n");
            return false;
        }
    }

    // if we are here, then we can safely init the window
    mWindow = mRoot->initialise(true, "AppTester Window");

    // creates a generic sceneManager
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    if (!mSceneMgr) {
        debugERROR("Could not construct the SceneManager\n");
        return false;

    }

    ////////////////////////////////////////////////////////////////////////////
    // load and initialize the resources
    debug("Loading resources\n");
    if (!loadResources()) {
        debugERROR("Error loading resources\n");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // create main light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    //setup the light
    l->setType(Ogre::Light::LT_DIRECTIONAL);
    l->setDirection(-0.5, 0, -0.5);

    ////////////////////////////////////////////////////////////////////////////
    // creates OIS Input
    debug("Creating mInputManager\n");
    core::size_t hWnd = 0;

    // override OIS construction to avoid grabbing mouse
    OIS::ParamList pl;
    core::size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    if (mDisableInputGrabbing) {
#if defined OIS_WIN32_PLATFORM
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_EXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
        pl.insert(
            std::make_pair(std::string("x11_mouse_grab"),
                std::string("false")));
        pl.insert(
            std::make_pair(std::string("x11_mouse_hide"),
                std::string("true")));
        pl.insert(
            std::make_pair(std::string("x11_keyboard_grab"),
                std::string("false")));
        pl.insert(
            std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
        // end override OIS construction to avoid grabbing mouse
    }

    mWindow->getCustomAttribute("WINDOW", &hWnd);
    //mInputManager = OIS::InputManager::createInputSystem(hWnd);
    mInputManager = OIS::InputManager::createInputSystem(pl);

    // creates the mouse and keyboard objects (we will use unbuffered input
    // system, the second parameter set this)
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(
        OIS::OISKeyboard, false));

    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(
        OIS::OISMouse, false));

    ASSERT(mMouse && mKeyboard);

    // config the mouse to the screen size
    // TODO: we have to changue this if we resize the windows.
    unsigned int width, height, depth;
    int left, top;
    mWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &mouseState = mMouse->getMouseState();
    mouseState.width = width;
    mouseState.height = height;

    ////////////////////////////////////////////////////////////////////////////
    // creates the sceneNode of the camera
    OGRELOG("Creating Camera and CameraHandler");
    mCamera = mSceneMgr->createCamera("Camera");
    mCamera->setQueryFlags(0);
    mCamera->setVisibilityFlags(0);

    // Position it at 500 in Z direction
    //mCamera->setPosition(Ogre::Vector3(0,0,80));
    // Look back along -Z
//	mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);

    mBodySceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
        "BodyScnNode");
    // up to the floor 10 units
    mBodySceneNode->translate(400, 60, 400);
    // create the camera scene node and attach it to the body 5 units up
    Ogre::Vector3 head(0, 5, 0);
    mCameraScnNode = mBodySceneNode->createChildSceneNode(head);
    mCameraScnNode->attachObject(mCamera);

    ////////////////////////////////////////////////////////////////////////////
    // creates a default viewport
    OGRELOG("Creating the viewport");
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    vp->setAutoUpdated(true);

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    ////////////////////////////////////////////////////////////////////////////
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // here we are ready to run the editor
    mReadyToRun = true;
    return true;
}

/******************************************************************************/
void
AppTester::loadScene(void)
{
    // do nothing
}

/******************************************************************************/
void
AppTester::loadAditionalResourcesFromFile(const Ogre::String &file,
    const Ogre::String &p)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    try {
        cf.load(file);
    } catch (...) {
        debugERROR("ERROR: file: %s not found\n", file.c_str());
        return;
    }

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String path = p;

    int last = path.size() - 1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\') {
        path.append("\\");
    }
#else
    if (last >= 0 && path[last] != '/') {
        path.append("/");
    }
#endif

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = path + i->second;
            //debugGREEN("DEBUG: %s -- %s -- %s\n",archName, typeName, secName);
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

/******************************************************************************/
AppTester::AppTester(float& globalTimeFrame,
                     bool disableInputGrabbing,
                     bool showConfig,
                     bool useAdditionalResources) :
    mCamera(0)
,   mCameraScnNode(0)
,   mInputManager(0)
,   mKeyboard(0)
,   mMouse(0)
,   mRoot(0)
,   mSceneMgr(0)
,   mWindow(0)
,   mAnimCamera(false)
,   mDefaultInput(true)
,   mReadyToRun(false)
,   mStopRunning(false)
,   mGlobalTimeFrame(globalTimeFrame)
,   mDisableInputGrabbing(disableInputGrabbing)
,   mShowFPS(true)
,   mFPSText(0)
{
    loadInitialConfig(showConfig);

    // Load the aditional resources.
    if (useAdditionalResources) {
        const char* strPath = std::getenv(RESOURCES_PATH);
        if (strPath == 0) {
            debugERROR("Error reading the environment var %s\n"
                "You have to configure your system... read the mails :)\n",
                RESOURCES_PATH);
        } else {
            // everything is fine
            std::string path = strPath;
            // append the resources.cfg
            core::OSHelper::addEndPathVar(path);

            const std::string fname = path + "resources.cfg";
            debugGREEN("Trying to parse the resource file %s\n", fname.c_str());
            loadAditionalResourcesFromFile(fname, path);
        }
    }

    loadScene();

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // configure the text for the PFS
    mFPSText = new OgreText;
    mFPSText->setPos(0.0f, 0.97f);
    mFPSText->setText("FPS: ");
    mFPSText->setColor(1.f, 0.f, 0.f, 1.f);
}

/******************************************************************************/
int
AppTester::run()
{
    if (!mReadyToRun) {
        OGRELOG("There was some problem when loading the initial configuration");
        return -1;
    }

    Ogre::Timer timer;
    float timeStamp = 0;
    Ogre::Real accumTimer = 0.0f;
    unsigned int numFrames = 0;

    // here is the main loop
    // TODO: fix the FrameElapsedTime and check how to get the ogres one.

    while (true) {
        timeStamp = timer.getMilliseconds();

        // handle local input		capture input
        mKeyboard->capture();
        mMouse->capture();

        if (mDefaultInput) {
            handleKeyboardInputCamera();
            handleMouseInputCamera();
        }

        handleKeyboardInput();
        handleMouseInput();

        // render the frame
        if (mStopRunning || !mRoot->renderOneFrame())
            break;

        update();

        // show the fps
        if (mShowFPS) {
            accumTimer += mGlobalTimeFrame;
            ++numFrames;
            if (accumTimer >= 1.f) {
                mFPSText->setText("FPS: " + Ogre::StringConverter::toString(numFrames));
                accumTimer = 0.f;
                numFrames = 0;
            }
        }

        // This must be called when we use the renderOneFrame approach
        Ogre::WindowEventUtilities::messagePump();

        mGlobalTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
    }

    return 0;
}

AppTester::~AppTester()
{
    // TODO: ...
    if (mKeyboard) {
        mInputManager->destroyInputObject(mKeyboard);
    }
    if (mMouse) {
        mInputManager->destroyInputObject(mMouse);
    }
    OIS::InputManager::destroyInputSystem(mInputManager);
    delete mFPSText;
}

}
