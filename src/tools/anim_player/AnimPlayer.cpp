/*
 * AnimPlayer.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include <memory>
#include <cstring>

#include <tinyxml/tinyxml.h>

#include <debug/PrimitiveDrawer.h>
#include <xml/XMLHelper.h>

#include "AnimPlayer.h"


// helper stuff
//
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
    buttons.reserve(18); // just in case :p

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

    return buttons;
}

}


namespace tool {


const Ogre::ColourValue AnimPlayer::EntityAnimations::ENABLED_ANIM_COL =
    Ogre::ColourValue::Green;
const Ogre::ColourValue AnimPlayer::EntityAnimations::DISABLED_ANIM_COL =
    Ogre::ColourValue::Red;
const Ogre::ColourValue AnimPlayer::EntityAnimations::SELECTED_ANIM_COL =
    Ogre::ColourValue::Blue;


////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::setText(unsigned int index)
{
    ASSERT(index < mAnims.size());
    Ogre::AnimationState* anim = mAnims[index];
    const Ogre::ColourValue& color = (anim->getEnabled()) ? ENABLED_ANIM_COL : DISABLED_ANIM_COL;
    const char* loopStr = anim->getLoop() ? "Looping" : "Not looping";

    // set the text
    mTable.setText(index+2, "Animation: " + anim->getAnimationName() + " " + loopStr, color);
}

////////////////////////////////////////////////////////////////////////////////
AnimPlayer::EntityAnimations::EntityAnimations(core::OgreTextTable& table) :
    mTable(table)
,   mCurrentAnim(0)
,   mEntity(0)
{}
AnimPlayer::EntityAnimations::~EntityAnimations()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
AnimPlayer::EntityAnimations::setEntity(Ogre::Entity* ent)
{
    ASSERT(ent);
    mEntity = ent;

    // first of all we will get all the animations of the entity
    Ogre::AnimationStateSet* anims = mEntity->getAllAnimationStates();
    if (!anims) {
        debugWARNING("The current mesh hasn't animations\n");
        return false;
    }
    Ogre::AnimationStateIterator animIt = anims->getAnimationStateIterator();
    mAnims.clear();
    while(animIt.hasMoreElements()){
        Ogre::AnimationState *anim = animIt.getNext();
        if (!anim) {
            break;
        }
        mAnims.push_back(anim);
    }

    if (mAnims.empty()) {
        debugWARNING("The current mesh hasn't animations\n");
        return false;
    }

    // now we will show all the animations and the current name of the mesh
    // and entity:
    // 0 -> Entity name
    // 1 -> Mesh Name
    //
    mTable.setNumRows(mAnims.size() + 2, 0.015f, 0, 0);
    mTable.setText(0, "EntityName: " + mEntity->getName());
    mTable.setText(1, "MeshName: " + mEntity->getMesh()->getName());

    // for all the other animations we will show their names
    //
    for(unsigned int i = 0, size = mAnims.size(); i < size; ++i) {
        mAnims[i]->setEnabled(false);
        mAnims[i]->setLoop(false);
        setText(i);
    }

    // set the selected row the SELECTED color
    mCurrentAnim = 0;
    mTable.setColor(mCurrentAnim+2, SELECTED_ANIM_COL);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::moveNextAnim(void)
{
    // change the color of the current anim to the specific one
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    mTable.setColor(mCurrentAnim+2,
        (anim->getEnabled()) ? ENABLED_ANIM_COL : DISABLED_ANIM_COL);
    mCurrentAnim = (mCurrentAnim + 1) % mAnims.size();
    mTable.setColor(mCurrentAnim+2, SELECTED_ANIM_COL);

}

////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::movePrevAnim(void)
{
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    mTable.setColor(mCurrentAnim+2,
        (anim->getEnabled()) ? ENABLED_ANIM_COL : DISABLED_ANIM_COL);

    if (mCurrentAnim > 0) {
        --mCurrentAnim;
    } else {
        mCurrentAnim = mAnims.size() - 1;
    }
    mTable.setColor(mCurrentAnim+2, SELECTED_ANIM_COL);
}

////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::startCurrentAnim(bool start)
{
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    if (anim->getEnabled() != start) {
        anim->setTimePosition(0);
        anim->setEnabled(start);
        setText(mCurrentAnim);
    }
}

bool
AnimPlayer::EntityAnimations::isStartedCurrentAnim(void) const
{
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    return anim->getEnabled();
}

////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::loopCurrentAnim(bool loop)
{
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    if (anim->getLoop() != loop) {
        anim->setLoop(loop);
        setText(mCurrentAnim);
    }
}
bool
AnimPlayer::EntityAnimations::isLoopCurrentAnim(void) const
{
    ASSERT(mCurrentAnim < mAnims.size());
    Ogre::AnimationState* anim = mAnims[mCurrentAnim];
    return anim->getLoop();
}

////////////////////////////////////////////////////////////////////////////////
void
AnimPlayer::EntityAnimations::updateAnims(float timeFrame)
{
    for (unsigned int i = 0, size = mAnims.size(); i < size; ++i) {
        Ogre::AnimationState* anim = mAnims[i];
        if (anim->getEnabled()) {
            anim->addTime(timeFrame);
        }
    }
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool
AnimPlayer::parseXML(const std::string& xmlFName, std::string& meshName) const
{
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <AnimPlayer mesh="file.mesh" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "AnimPlayer") != 0) {
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

////////////////////////////////////////////////////////////////////////////////
bool
AnimPlayer::loadEntity(const std::string& meshName)
{
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    try {
        mEntity = mSceneMgr->createEntity(meshName.c_str());
    } catch(...) {
        debugERROR("Mesh %s not found!\n", meshName.c_str());
        return false;
    }
    mNode->attachObject(mEntity);

    // try to load the animations now
    return mEntityAnims.setEntity(mEntity);
}

void
AnimPlayer::handleCameraInput()
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
        mOrbitCamera.setCameraType(OrbitCamera::CameraType::FreeFly);
    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
        mOrbitCamera.setCameraType(OrbitCamera::CameraType::Orbit);
    }

}

AnimPlayer::AnimPlayer() :
    core::AppTester(mTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mEntityAnims(mTextTable)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);
}

AnimPlayer::~AnimPlayer()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
AnimPlayer::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    std::string meshName;
    if (!parseXML(ANIM_PLAYER_XML_FILE, meshName)) {
        // nothing to do
        return;
    }
    if (!loadEntity(meshName)) {
        // nothing to do
        return;
    }

    // everything fine...
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
AnimPlayer::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update the animations
    mEntityAnims.updateAnims(mTimeFrame);

    // check for the input to handle the selection
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_L)) {
        mEntityAnims.loopCurrentAnim(!mEntityAnims.isLoopCurrentAnim());
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_E)) {
        mEntityAnims.startCurrentAnim(!mEntityAnims.isStartedCurrentAnim());
    }

    // check if we want to move to the next/preve anim
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_ADD)) {
        mEntityAnims.moveNextAnim();
    } else if (mInputHelper.isKeyReleased(input::KeyCode::KC_MINUS)) {
        mEntityAnims.movePrevAnim();
    }

    // update the camera
    handleCameraInput();
}

}
