/*
 * Coll2DTestTool.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreNode.h>

#include <memory>
#include <cstring>
#include <fstream>
#include <deque>
#include <map>
#include <set>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <ogre_utils/DotSceneLoader.h>
#include <ogre_utils/OgreUtil.h>
#include <types/BitMatrix.h>
#include <collisions/helpers/CollStaticWorldLoader.h>


#include "Coll2DTestTool.h"


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
    buttons.push_back(input::KeyCode::KC_SPACE);
    buttons.push_back(input::KeyCode::KC_LSHIFT);
    buttons.push_back(input::KeyCode::KC_Z);
    buttons.push_back(input::KeyCode::KC_B);

    return buttons;
}

// Create a string from a vector
//
Ogre::String
toString(const Ogre::Vector3& pos)
{
    return Ogre::StringConverter::toString(pos);
}

}


namespace tool {


////////////////////////////////////////////////////////////////////////////////
bool
Coll2DTestTool::parseXML(const std::string& xmlFName, std::string& scene) const
{
    std::unique_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <Coll2DTestTool scene="file.scene" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "Coll2DTestTool") != 0) {
        debugERROR("Invalid xml root element name\n");
        return false;
    }

    // check now for the value we want
    if (root->Attribute("scene") == 0) {
        debugERROR("scene attributes not found in the xml file... "
            "invalid xml\n");
        return false;
    }

    // everything fine
    scene = root->Attribute("scene");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Coll2DTestTool::loadScene(const std::string& scene)
{


    // always fine?
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Coll2DTestTool::createCollObjects(const std::string& scene)
{
    // create the scene node for all the childs
    mCollObjectsNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    // set the zone for where we will be moving on
    Ogre::AxisAlignedBox moveZone(-8000,
                                  -8000,
                                  50,
                                  8000,
                                  8000,
                                  8000);

    // configure the velocity taking into account the size of the level
    mSatelliteCamera.configure(moveZone, 10);


    // read the .scene and export the information into a file
    if (!coll::CollStaticWorldLoader::exportFromScene(scene,
                                                      mSceneMgr,
                                                      "level.sco")){
        debugERROR("Error exporting the scene %s\n", scene.c_str());
        return false;
    }

    // import the file
    coll::CollStaticWorldLoader::LevelInfo li;
    if (!coll::CollStaticWorldLoader::importFromFile("level.sco", li)){
        debugERROR("Error importing the sco file\n");
        return false;
    }

    // increase the world a little bit
    static const float worldDelta = 100.f;
    core::AABB worldBB = li.staticsBB;
    worldBB.tl.x -= worldDelta;
    worldBB.tl.y += worldDelta;
    worldBB.br.y -= worldDelta;
    worldBB.br.x += worldDelta;
    mCollHandler.setWorldBoundingBox(worldBB);

    // create the collision objects and show them
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    coll::CollPreciseInfoBuilder builder(&mCollHandler);
    std::vector<Ogre::Vector3> points;
    points.reserve(512);
    mPrimitives.reserve(512);
    for (unsigned int i = 0; i < li.indices.size(); ++i) {
        points.clear();
        for (unsigned int v = li.indices[i].first, count = li.indices[i].second;
                v < count; ++v) {
            const core::Vector2& current = li.vertices[v];
            points.push_back(Ogre::Vector3(current.x, current.y, 0));
        }
        points.push_back(Ogre::Vector3(li.vertices[li.indices[i].first].x,
                                       li.vertices[li.indices[i].first].y,
                                       0));

        core::Primitive* p = pd.createMultiline(points);
        mPrimitives.push_back(TmpPrimitiveColor(p, pd.getFreshColour()));

        // create the collision object for this one
        const unsigned int beg = li.indices[i].first,
                           end = li.indices[i].second;

        builder.setInfo(li.vertices.begin() + beg, end - beg);
        coll::CollObject* co = builder.constructCollObject();
        if (co == 0) {
            debugERROR("Error constructing the collision object for mesh\n");
            return false;
        }
        // NOTE Dangerous this, we are assuming we are not reallocating memory
        // in mPrimitives.
        co->setUserDef(&(mPrimitives.back()));
        mPrimitives.back().collObj = co;

        // add this collision object to the handler as static object
        mCollHandler.coAddStatic(co);
    }

    // construct the player of size... I don't know...
    static const float playerHalfSize = 15.f;
    core::AABB pbb(playerHalfSize, -playerHalfSize, -playerHalfSize, playerHalfSize);
    mPlayer = mCollHandler.getNewCollObject(~0, pbb);
    mPlayerPrim = pd.createPlane(Ogre::Vector3(0,0, points.back().z + 2.f),
                                 Ogre::Vector2(pbb.getWidth(),pbb.getHeight()),
                                 Ogre::ColourValue::Green);

    // add the player as a dynamic object
    mCollHandler.coAddDynamic(mPlayer);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
Coll2DTestTool::handlePlayerInput(void)
{
    ASSERT(mPlayerPrim);

    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
        mTranslationVec.y += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
        mTranslationVec.y -= 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mTranslationVec *= (200.f * mGlobalTimeFrame);
        mPlayer->translate(core::Vector2(mTranslationVec.x, mTranslationVec.y));
        const core::Vector2 center = mPlayer->center();
        mPlayerPrim->node->setPosition(center.x, center.y, mPlayerPrim->node->getPosition().z);

    }
}

////////////////////////////////////////////////////////////////////////////////
void
Coll2DTestTool::handlePlayerCollisions(void)
{
    coll::QueryArgs args;
    coll::QueryResult result;

    // perform accurate colls
    args.ptype = coll::PrecisionType::CQ_PreciseCheck;

    if (!mCollHandler.performQuery(mPlayer, args, result)) {
        // no collisions, reset the colours
        for (unsigned int i = 0; i < mLastColls.size(); ++i) {
            ASSERT(mLastColls[i]->userDef());
            TmpPrimitiveColor* prim = static_cast<TmpPrimitiveColor*>(mLastColls[i]->userDef());
            prim->resetColor();
        }
        mLastColls.clear();
        return; // no collisions
    }

    // we collide with something, mark them as red and put it in the vector
    // if and only if they weren't already collided.
    ASSERT(!result.objects.empty());
    coll::QueryResultVec& objs = result.objects;
    for (unsigned int i = 0; i < objs.size(); ++i) {

        // check if the object already exists or not in the mLastColls vector
        if (std::find(mLastColls.begin(), mLastColls.end(), objs[i]) == mLastColls.end()) {
            ASSERT(objs[i]->userDef());
            TmpPrimitiveColor* prim = static_cast<TmpPrimitiveColor*>(objs[i]->userDef());
            mLastColls.push_back(objs[i]);
            prim->setTmpColor(Ogre::ColourValue::Red);
        }
    }

    // check now for the elements that are not colliding any more and remove
    // them
    for (unsigned int i = 0; i < mLastColls.size(); ++i) {
        if (std::find(objs.begin(), objs.end(), mLastColls[i]) == objs.end()) {
            // we need to remove this object from mLastColls
            TmpPrimitiveColor* prim = static_cast<TmpPrimitiveColor*>(mLastColls[i]->userDef());
            prim->resetColor();
            mLastColls.disorder_remove(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
Coll2DTestTool::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x -= 1.0f;
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.y -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.y += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_ADD)) {
        mTranslationVec.z -= 5.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_MINUS)) {
        mTranslationVec.z += 5.0f;
    }

    const float lMouseZ = float(input::Mouse::relZ());
    if (lMouseZ > 0.0f) {
        mTranslationVec.z += 10.f;
    } else if (lMouseZ < 0.0f) {
        mTranslationVec.z -= 10.f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mSatelliteCamera.translate(mTranslationVec * (200.f * mGlobalTimeFrame));
    }
}

Coll2DTestTool::Coll2DTestTool() :
    core::AppTester(mTimeFrame)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mSelectionHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mCollObjectsNode(0)
,   mPlayer(0)
,   mPlayerPrim(0)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);

    setUseDefaultInput(false);

    // configure the mouse cursor
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());

    mMousePosText.setPos(0,0);
    mMousePosText.setColor(0, 1, 1, 1.f);
    mPlayerPosText.setPos(0,0.1f);
    mPlayerPosText.setColor(0, 1, 1, 1.f);

}

Coll2DTestTool::~Coll2DTestTool()
{

}

/* Load additional info */
void
Coll2DTestTool::loadAditionalData(void)
{
    // configure the plane for the seleciton system
    Ogre::Plane basePlane(Ogre::Vector3::UNIT_Z, 0.0f);
    mSelectionHelper.setBaseIntersectionPlane(basePlane);

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    std::string scene;
    if (!parseXML(COLLTESTTOOL_TOOL_XML_FILE, scene)) {
        // nothing to do
        return;
    }

    if (!loadScene(scene)) {
        // nothing to do
        return;
    }

    // build all the collision objects
    if (!createCollObjects(scene)) {
        debugERROR("Some error occur when trying to create the collisions objects "
            "for the scene: %s\n", scene.c_str());
        return;
    }
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
Coll2DTestTool::update()
{
    // show player position
    mPlayerPosText.setText("PlayerPos: " + toString(mPlayerPrim->node->getPosition()), 0.015f);

    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // update mouse cursor
    mMouseCursor.updatePosition(input::Mouse::absX(), input::Mouse::absY());

    // update the camera
    handleCameraInput();

    // update the player movement
    handlePlayerInput();

    // here we update the collisions world
    mCollHandler.update();

    // check for the collisions of the player
    handlePlayerCollisions();

    // hide show the elements of the scene / primitives
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_1)) {
        // show the real meshes
        ASSERT(mCollObjectsNode);
        mCollObjectsNode->flipVisibility();
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_2)) {
        // show the real meshes
        for (unsigned int i = 0, size = mPrimitives.size(); i < size; ++i) {
            mPrimitives[i].primitive->flipVisibility();
        }
    }
}

}
