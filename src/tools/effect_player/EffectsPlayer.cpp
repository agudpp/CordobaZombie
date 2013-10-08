/*
 * EffectsPlayer.cpp
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

#include "EffectsPlayer.h"


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
    buttons.push_back(input::KeyCode::KC_3);
    buttons.push_back(input::KeyCode::KC_4);
    buttons.push_back(input::KeyCode::KC_5);
    buttons.push_back(input::KeyCode::KC_6);
    buttons.push_back(input::KeyCode::KC_7);
    buttons.push_back(input::KeyCode::KC_8);
    buttons.push_back(input::KeyCode::KC_9);
    buttons.push_back(input::KeyCode::KC_E);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_SPACE);

    return buttons;
}

// @brief Parse a OgreVector from an xml
// @param elem      The xml element
// @param vec       The vector
// @return true on success || false otherwise
//
inline bool
parseVector(const TiXmlElement* elem, Ogre::Vector3& vec)
{
    ASSERT(elem);
    const char* x = elem->Attribute("x");
    const char* y = elem->Attribute("y");
    const char* z = elem->Attribute("z");

    if (x == 0 || y == 0 || z == 0) {
        return false;
    }
    core::XMLHelper::parseFloat(elem, "x", vec.x);
    core::XMLHelper::parseFloat(elem, "y", vec.y);
    core::XMLHelper::parseFloat(elem, "z", vec.z);
    return true;
}

}


namespace tool {

////////////////////////////////////////////////////////////////////////////////
bool
EffectsPlayer::parseXML(const std::string& xml,
                        std::vector<ParticleInfo>& particleNames) const
{
    std::unique_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xml.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xml.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <EffectsPlayer>
    //      <ParticleSystem name="particle1" x="0" y="0" z="0"/>
    //      <ParticleSystem name="particle2" x="0" y="0" z="0"/>
    // </EffectsPlayer>
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "EffectsPlayer") != 0) {
        debugERROR("Invalid xml root element name\n");
        return false;
    }

    // get each of the elements
    std::vector<const TiXmlElement *> elements;
    core::XMLHelper::getFirstElements(root, elements);
    if (elements.empty()) {
        debugWARNING("No ParticleSystem were found in the .xml file\n");
        return false;
    }

    if (elements.size() > 9) {
        debugWARNING("WARNING: There are more than 9 particles systems, we support "
            "only 9 for now\n");
    }

    // now we will parse each one
    for (unsigned int i = 0, size = elements.size(); i < size && i < 9; ++i) {
        const TiXmlElement* elem = elements[i];
        ASSERT(elem);
        if (std::strcmp(elem->Value(), "ParticleSystem") != 0) {
            debugERROR("Some of the nodes of the xml is wrong, the name is %s and"
                " the expected name is %s\n", elem->Value(), "ParticleSystem");
            return false;
        }
        const char* name = elem->Attribute("name");
        Ogre::Vector3 vec;
        if (elem->Attribute("name") == 0 || !parseVector(elem, vec)) {
            debugERROR("Invalid element node in the xml\n");
            return false;
        }
        ParticleInfo pi;
        pi.name = name;
        pi.position = vec;
        particleNames.push_back(pi);
    }

    return true;
}

void
EffectsPlayer::handleCameraInput()
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
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_SPACE)) {
        if (mOrbitCamera.cameraType() == tool::OrbitCamera::CameraType::FreeFly) {
            mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
        } else {
            mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
        }
    }

}

EffectsPlayer::EffectsPlayer() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, cz::GlobalData::lastTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // configure global data
    cz::GlobalData::sceneMngr = mSceneMgr;

}

EffectsPlayer::~EffectsPlayer()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
EffectsPlayer::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // load the xml with all the particles we need
    std::vector<ParticleInfo> pinfo;
    if (!parseXML(EFfECTS_PLAYER_XML, pinfo)) {
        debugERROR("Some error occur loading the xml\n");
        return;
    }

    mTextTable.setNumRows(pinfo.size(), 0.015f, 0.f, 0.f);

    // load all the particle systems
    for (unsigned int i = 0; i < pinfo.size(); ++i) {
        ParticleInfo& pi = pinfo[i];

        // try to load the particle system if it is not already loaded
        if (mSceneMgr->hasParticleSystem(pi.name)) {
            debugWARNING("Trying to load the same particle system multiple times:"
                " %s\n", pi.name.c_str());
            continue;
        }
        Ogre::ParticleSystem* ps = mSceneMgr->createParticleSystem(pi.name, pi.name);
        if (ps == 0) {
            debugERROR("Error creating particle system with name %s\n", pi.name.c_str());
            continue;
        }

        ParticleSystemData pd;
        pd.ps = ps;
        pd.node = mSceneMgr->createSceneNode();
        pd.node->setPosition(pi.position);
        pd.node->attachObject(pd.ps);
        Ogre::String text = "\tKey ";
        text.append(Ogre::StringConverter::toString(i+1));
        text.append(" : ");
        text.append(pi.name);
        mTextTable.setText(i, text, Ogre::ColourValue::Blue);

        debugGREEN("Loading particle system: %s\n", pi.name.c_str());

        // load it
        mParticles.push_back(pd);
    }

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
EffectsPlayer::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // check for each key now
    for (int i = 0; i < mParticles.size(); ++i) {
        if (mInputHelper.isKeyReleased(input::KeyCode(input::KeyCode::KC_1 + i))) {
            // check if we have to show it or remove it
            if (mParticles[i].node->getParentSceneNode()) {
                // remove it
                mParticles[i].node->getParentSceneNode()->removeChild(mParticles[i].node);
                mParticles[i].ps->setEmitting(false);
                mTextTable.setColor(i, Ogre::ColourValue::Blue);
            } else {
                // attach / show it
                mSceneMgr->getRootSceneNode()->addChild(mParticles[i].node);
                mParticles[i].ps->setEmitting(true);
                mTextTable.setColor(i, Ogre::ColourValue::Green);
            }
        }
    }

    // update the camera
    handleCameraInput();

}

}
