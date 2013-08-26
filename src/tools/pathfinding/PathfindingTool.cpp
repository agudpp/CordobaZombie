/*
 * PathfindingTool.cpp
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
#include <fstream>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <ogre_utils/DotSceneLoader.h>
#include <pathfinding/WayPointGraphBuilder.h>
#include <types/BitMatrix.h>

#include "PathfindingTool.h"


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

// Create a string from a vector
//
Ogre::String
toString(const Ogre::Vector3& pos)
{
    return Ogre::StringConverter::toString(pos);
}


}


namespace tool {

// default z value
static const float startingZ = 35.0f;


////////////////////////////////////////////////////////////////////////////////
bool
PathfindingTool::parseXML(const std::string& xmlFName,
                          std::string& scene,
                          std::string& waypoints) const
{
    std::shared_ptr<TiXmlDocument> doc(core::XMLHelper::loadXmlDocument(xmlFName.c_str()));
    if (doc.get() == 0) {
        debugERROR("Error reading the file %s\n", xmlFName.c_str());
        return false;
    }

    // we could read the file properly, get the element we want
    // <PathfindingTool scene="file.scene" waypoints="waypoints.txt" />
    //
    const TiXmlElement* root = doc->RootElement();
    if (!root) {
        debugERROR("Couldn't get the root element\n");
        return false;
    }

    // check if it is the want we want
    if (root->Value() == 0 || std::strcmp(root->Value(), "PathfindingTool") != 0) {
        debugERROR("Invalid xml root element name\n");
        return false;
    }

    // check now for the value we want
    if (root->Attribute("scene") == 0 || root->Attribute("waypoints") == 0) {
        debugERROR("scene or waypoints attributes not found in the xml file... "
            "invalid xml\n");
        return false;
    }

    // everything fine
    scene = root->Attribute("scene");
    waypoints = root->Attribute("waypoints");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PathfindingTool::loadScene(const std::string& scene)
{
    // set the query masks for the movable object
    Ogre::MovableObject::setDefaultQueryFlags(0xFFFF);

    Ogre::DotSceneLoader dsl;
    dsl.parseDotScene(scene,
                      Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                      mSceneMgr);

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
PathfindingTool::loadWaypoints(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ifstream::in);

    if (!file) {
        debugERROR("File %s couldn't be read\n", filename.c_str());
        return false;
    }

    // now parse the info
    mWaypoints.clear();
    mWaypoints.reserve(512);
    gps::Vertex v;
    while ((file >> v.x) && (file >> v.y)) {
        mWaypoints.push_back(v);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PathfindingTool::buildGraph(void)
{
    if (mWaypoints.empty()) {
        debugWARNING("There are no waypoints to build the graph\n");
        return false;
    }

    // now we will perform a raycast between each possible pair of waypoints
    // and check if they are able to see between them.
    // first we will initialize the nodes
    //
    gps::WayPointGraphBuilder wpgb;
    wpgb.setWaypointPosition(mWaypoints);

    Ogre::RaySceneQueryResultEntry entry;
    for (unsigned int i = 0, size = mWaypoints.size(); i < size; ++i) {
        for (unsigned int j = i+1; j < size; ++j) {
            const Ogre::Vector3 wp1(mWaypoints[i].x, mWaypoints[i].y, startingZ);
            const Ogre::Vector3 wp2(mWaypoints[j].x, mWaypoints[j].y, startingZ);

            // check if they can see between them
            if (!mSelectionHelper.performRaycast(wp1, wp2, entry)) {
                debugGREEN("adding link\n");
                wpgb.linkWaypoints(i, j);
            }
        }
    }

    // now we build the graph
    return wpgb.buildGraph(mGraph);
}

////////////////////////////////////////////////////////////////////////////////
void
PathfindingTool::drawGraph(const gps::WayPointGraph& graph,
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
    debugGREEN("Drawing a graph of size %d\n", size);
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
core::Primitive*
PathfindingTool::drawPath(const gps::WayPointPath& path,
                          const Ogre::ColourValue& color)
{
    ASSERT(path.size > 1);

    // we will just draw this as a multiline primitive
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    std::vector<Ogre::Vector3> points;
    points.resize(path.size);
    for (unsigned int i = 0; i < path.size; ++i) {
        points[i].x = path.node[i].x;
        points[i].y = path.node[i].x;
        points[i].z = startingZ + 0.5;
    }

    return pd.createMultiline(points, color);
}

////////////////////////////////////////////////////////////////////////////////
bool
PathfindingTool::pickPoint(Ogre::Vector3& post)
{
    return mSelectionHelper.getPlaneIntersection(post);
}

////////////////////////////////////////////////////////////////////////////////
void
PathfindingTool::handleCameraInput()
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

PathfindingTool::PathfindingTool() :
    core::AppTester(mTimeFrame)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mSelectionHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mVisualPath(0)
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
}

PathfindingTool::~PathfindingTool()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
PathfindingTool::loadAditionalData(void)
{
    // configure the plane for the seleciton system
    Ogre::Plane basePlane(Ogre::Vector3::UNIT_Z, 0.0f);
    mSelectionHelper.setBaseIntersectionPlane(basePlane);

    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    std::string scene, waypoints;
    if (!parseXML(PATHFINDING_TOOL_XML_FILE, scene, waypoints)) {
        // nothing to do
        return;
    }
    if (!loadScene(scene) || !loadWaypoints(waypoints)) {
        // nothing to do
        return;
    }

    // render one frame to be able to perform raycasts
    mRoot->renderOneFrame();
    mSceneMgr->getRootSceneNode()->showBoundingBox(true);

    // build the graph and draw it
    if (!buildGraph()) {
        // nothing to do?
        return;
    }
    drawGraph(mGraph);

    // everything fine...

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
PathfindingTool::update()
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

    // check if we hit some position
    Ogre::Vector3 hitPos;
    if (!mSelectionHelper.getPlaneIntersection(hitPos)) {
        // nothing to do
        return;
    }
    mMousePosText.setText("MapPosition: " + toString(hitPos), 0.023f);


    // check for positions selections
    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left) &&
            mGraph.nodes().size > 0) {
        // check if is the first or the second position
        if (mSelectedPos.size() == 0) {
            // is the first one only
            mSelectedPos.push_back(core::Vector2(hitPos.x, hitPos.y));
        } else {
            mSelectedPos.push_back(core::Vector2(hitPos.x, hitPos.y));
            // is the second one, we need to do the pathfinding and
            // show the path
            gps::WayPointPath path;
            if (!mPathfinder.findPath(mSelectedPos[0], mSelectedPos[1], path)){
                mSelectedPos.clear();
                debugERROR("Path not found\n");
                return;
            }
            mSelectedPos.clear();

            // draw the path
            core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
            pd.deletePrimitive(mVisualPath);
            mVisualPath = drawPath(path, pd.getFreshColour());
        }
    }
}

}
