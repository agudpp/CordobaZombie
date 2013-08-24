/*
 * AppTester.h
 *
 *  Created on: 23/12/2010
 *      Author: agustin
 */

#ifndef AppTester_H_
#define AppTester_H_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreMeshManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreTimer.h>
#include <OgreSceneQuery.h>
#include <OgreColourValue.h>
// OIS
#include <OIS.h>
// std
#include <string>
/* own libs */

#include <debug/DebugUtil.h>
#include <debug/OgreText.h>

/* Here we will define the default values of the resources and plugins config
 * files */
#define PLUGINS_CONF_FILE		"plugins.cfg"
#define RESOURCES_CONF_FILE		"resources.cfg"

#define RESOURCES_PATH	"CZ01_RC_PATH"

namespace core {

class AppTester
{
public:
    /* @brief Construct all the Ogre environment needed to run a simple app.
     *        This will construct a generic scene manager and the default values
     *        for all the other stuff
     */
    AppTester(float& globalTimeFrame,
              bool disableInputGrabbing = true,
              bool showConfig = true,
              bool useAdditionalResources = true);

    /* run the editor */
    int
    run();

    /* Load additional info */
    virtual void
    loadAditionalData(void)
    {}

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    virtual void
    update()
    {}

    // Use default input
    void
    setUseDefaultInput(bool d)
    {
        mDefaultInput = d;
    }

    // show the FPS at the botton left corner
    //
    inline void
    showFPS(bool show) {mShowFPS = show;}

    virtual
    ~AppTester();

    Ogre::RenderWindow *getRenderWindow(void) {return mWindow;}
    Ogre::SceneManager *getSceneManager(void) {return mSceneMgr;}
    Ogre::Root* getRoot(void) {return mRoot;}

    OIS::InputManager *getInputManager(void) {return mInputManager;}
    OIS::Keyboard * getKeyboard(void) {return mKeyboard;}
    OIS::Mouse *getMouse(void) {return mMouse;}

    // camera
    Ogre::Camera *getCamera(void) {return mCamera;}
    Ogre::SceneNode *getCamSceneNode(void) {return mCameraScnNode;}

protected:
    /* Function to setup the first configurations */
    bool
    loadResources(void);
    bool
    loadInitialConfig(bool showConfig);

    /* Function to handle the input (select the objects, move the elements,
     * resize the elements, load objects, remove objects, etc)
     */
    void
    handleKeyboardInputCamera(void);
    void
    handleMouseInputCamera(void);

    /* Load world */
    void
    loadScene(void);

    /* Implement this clas */
    virtual void
    handleKeyboardInput(void)
    {}

    virtual void
    handleMouseInput(void)
    {}

    void
    loadAditionalResourcesFromFile(const Ogre::String &fname,
        const Ogre::String &path = "");

private:
    AppTester(const AppTester &);

protected:
    Ogre::Root *mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;

    // input
    OIS::InputManager *mInputManager;
    OIS::Keyboard *mKeyboard;
    OIS::Mouse *mMouse;

    bool mDefaultInput;
    bool mDisableInputGrabbing;

    // camera
    Ogre::Camera *mCamera;
    Ogre::SceneNode *mCameraScnNode;
    Ogre::SceneNode *mBodySceneNode;

    // general
    Ogre::Plane mGlobalPlane;

    bool mReadyToRun;
    bool mStopRunning;

    bool mAnimCamera;
    float &mGlobalTimeFrame;

    bool mShowFPS;
    OgreText* mFPSText;

};

}

#endif /* AppTester_H_ */
