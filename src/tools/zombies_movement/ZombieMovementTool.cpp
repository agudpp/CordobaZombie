/*
 * ZombieMovementTool.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <OgreSkeletonManager.h>

#include <memory>
#include <cstring>
#include <fstream>
#include <deque>
#include <map>
#include <set>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <ogre_utils/DotSceneLoader.h>
#include <pathfinding/WayPointGraphBuilder.h>
#include <types/BitMatrix.h>
#include <collisions/helpers/CollStaticWorldLoader.h>

#include "ZombieMovementTool.h"


// helper stuff
//
namespace {


// default z value
static const float startingZ = 25.0f;
static const Ogre::uint32 LEVEL_OBJECTS_MASK = 0x1;
static const Ogre::uint32 GRAPH_OBJECTS_MASK = 0x2;



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
    buttons.push_back(input::KeyCode::KC_LCONTROL);
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
ZombieMovementTool::parseXML(const std::string& xmlFName,
                          std::string& scene,
                          std::string& graphFile,
                          std::string& collScene) const
{
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <ZombieMovementTool scene="file.scene" graphFile="graph.wpg" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "ZombieMovementTool") != 0) {
        debugERROR("Invalid xml root element name\n");
        return false;
    }

    // check now for the value we want
    if (root->Attribute("scene") == 0 ||
        root->Attribute("graphFile") == 0 ||
        root->Attribute("collScene") == 0) {
        debugERROR("scene or graphFile attributes not found in the xml file... "
            "invalid xml\n");
        return false;
    }

    // everything fine
    scene = root->Attribute("scene");
    graphFile = root->Attribute("graphFile");
    collScene = root->Attribute("collScene");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieMovementTool::loadScene(const std::string& scene)
{
    // set the query masks for the movable object
    Ogre::MovableObject::setDefaultQueryFlags(LEVEL_OBJECTS_MASK);

    Ogre::DotSceneLoader dsl;
    dsl.parseDotScene(scene,
                      Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                      mSceneMgr,
                      mSceneMgr->getRootSceneNode()->createChildSceneNode());

    // set the zone for where we will be moving on
    Ogre::AxisAlignedBox moveZone(-8000,
                                  -8000,
                                  50,
                                  8000,
                                  8000,
                                  8000);
    // configure the velocity taking into account the size of the level
    mSatelliteCamera.configure(moveZone, 10);

    // always fine?
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieMovementTool::loadGraph(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ifstream::in);

    if (!file) {
        debugERROR("File %s couldn't be read\n", filename.c_str());
        return false;
    }

    // now parse the info
    return mGraph.build(filename);
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieMovementTool::drawGraph(const gps::WayPointGraph& graph,
                           const Ogre::ColourValue& color)
{
    // we will draw this in a very ugly way but doesn't matter, is just for debug
    //
    core::BitMatrix<> adjacency;
    adjacency.build(graph.nodes().size, graph.nodes().size);
    adjacency.reset();
    const gps::WayPointNode* nodes = graph.nodes().data;
    unsigned int size = graph.nodes().size;

    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    for (unsigned int i = 0; i < size; ++i) {
        const gps::WayPointNode& n = nodes[i];
        for (unsigned int j = 0; j < n.neighborsCount; ++j) {
            // check if this link was already set
            if (adjacency.get(i, n.neighbors[j])) {
                continue;
            }

            // add the link and mark it
            const gps::WayPointNode& neigh = nodes[n.neighbors[j]];
            adjacency.set(i, n.neighbors[j], true);
            adjacency.set(n.neighbors[j], i, true);
            pd.createLine(Ogre::Vector3(n.position.x, n.position.y, startingZ - 1.f),
                          Ogre::Vector3(neigh.position.x, neigh.position.y, startingZ - 1.f),
                          color);
        }
        pd.createBox(Ogre::Vector3(n.position.x, n.position.y, startingZ),
                     Ogre::Vector3(15,15,15),
                     Ogre::ColourValue::Red);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieMovementTool::createCollObjects(const std::string& scene)
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
    std::cout << "WorldSize: " << worldBB <<std::endl;
    debugERROR("remove return\n");
    return true;



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

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieMovementTool::createZombies(void)
{
    /*Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ent = mSceneMgr->createEntity("tronco.mesh");
    ASSERT(ent);

    // load the squeleton
    Ogre::SkeletonPtr skeleton =
        Ogre::SkeletonManager::getSingleton().load("zombiefeo.skeleton",
            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    ASSERT(skeleton.get());
    Ogre::MeshPtr mesh =  ent->getMesh();
    ASSERT(mesh.get());
    mesh->setSkeletonName(skeleton->getName());

    node->attachObject(ent);
    mZombie.configure(node, ent);
    mZombie.setVelocity(135);

    // set the position we want
    const core::Vector2 pos(1500, 600);
    mZombie.setPosition(pos);

    // make the zombie born!
    mZombie.born();

    // find the path for him
    mZombie.moveTo(core::Vector2(0,0));*/


    mZombies.resize(100);
    for (unsigned int i = 0; i < mZombies.size(); ++i) {
        Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        Ogre::Entity* ent = mSceneMgr->createEntity("tronco.mesh");
        mZombies[i] = new cz::ZombieUnit;
        cz::ZombieUnit& z = *mZombies[i];
        z.configure(node, ent);
        z.setVelocity(135);

        float bx = Ogre::Math::RangeRandom(-10, 2200);
        float by = Ogre::Math::RangeRandom(-100, 2300);
        float mx = Ogre::Math::RangeRandom(-10, 2200);
        float my = Ogre::Math::RangeRandom(-100, 2300);
//        float bx = Ogre::Math::RangeRandom(-10, -11);
//        float by = Ogre::Math::RangeRandom(-100, -101);
//        float mx = Ogre::Math::RangeRandom(2200, 2200);
//        float my = Ogre::Math::RangeRandom(2300, 2300);
        z.setPosition(core::Vector2(bx,by));
        z.born();
        z.moveTo(core::Vector2(mx, my));
    }
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieMovementTool::handleCameraInput()
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

ZombieMovementTool::ZombieMovementTool() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mData()
,   mTimeFrame(cz::GlobalData::lastTimeFrame)
,   mPathfinder(mData.pathfinder)
,   mCollHandler(mData.collHandler)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
{
    ASSERT(cz::GlobalData::collHandler);

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

    mStatusBar.setPos(0,0.017f);
    mStatusBar.setColor(0, 1, 1, 1.f);
}

ZombieMovementTool::~ZombieMovementTool()
{

}

/* Load additional info */
void
ZombieMovementTool::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    std::string scene, graphFile, collScene;
    if (!parseXML(ZOMBIEMOVEMENET_TOOL_XML_FILE, scene, graphFile, collScene)) {
        // nothing to do
        return;
    }
    if (!scene.empty() && !loadScene(scene)) {
        // nothing to do
        return;
    }

    // render one frame to be able to perform raycasts
    mRoot->renderOneFrame();
    mSceneMgr->getRootSceneNode()->showBoundingBox(true);

    // try to load the graph from a file
    if (graphFile.empty() || !loadGraph(graphFile)) {
        // some error occur
        debugWARNING("No graph could be loaded, you can create a new one now\n");
    } else {
        // we could load the graph correctly
        drawGraph(mGraph);
        mPathfinder.setGraph(&mGraph);
    }

    // load the coll objects
    if (collScene.empty() || !createCollObjects(collScene)) {
        debugWARNING("Empty collision scene!\n");
    }


    // create zombies
    createZombies();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
ZombieMovementTool::update()
{
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

    // update the collisions
    mCollHandler.update();

    // update the zombie logic
    if (mZombie.isAlive()) {
//        mZombie.update();
    }
    for (unsigned int i = 0; i < mZombies.size(); ++i) {
        cz::ZombieUnit& z = *mZombies[i];
        if (z.isAlive()) {
            z.update();
            if (!z.pathHandler().morePointsToGo()) {
                z.moveTo(core::Vector2(Ogre::Math::RangeRandom(-10, 2200),
                                       Ogre::Math::RangeRandom(-100, 2300)));
            }
        }
    }

}

}
