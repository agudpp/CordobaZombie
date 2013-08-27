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
#include <deque>
#include <map>
#include <set>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <ogre_utils/DotSceneLoader.h>
#include <pathfinding/WayPointGraphBuilder.h>
#include <types/BitMatrix.h>

#include "PathfindingTool.h"


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


////////////////////////////////////////////////////////////////////////////////
//                  Helper classes used to build the graph                    //
////////////////////////////////////////////////////////////////////////////////

// Graphical classes representing the edges and nodes
//
class GraphicEdge;
struct GraphicNode {
    core::Primitive* primitive;
    core::Vector2 position;
    std::set<GraphicEdge*> edges;
};

struct GraphicEdge {
    core::Primitive* primitive;
    GraphicNode* n1;
    GraphicNode* n2;
};
typedef std::shared_ptr<GraphicNode> GraphicNodePtr;
typedef std::shared_ptr<GraphicEdge> GraphicEdgePtr;

struct GraphicGraph {
    // create a new node
    bool createNode(const Ogre::Vector3& p, GraphicNode*& result) {
        core::Vector2 position(p.x, p.y);
        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

        GraphicNodePtr node(new GraphicNode);
        node->position = position;
        core::Primitive* prim = pd.createBox(p, Ogre::Vector3(55,55,55), Ogre::ColourValue::Red);
        node->primitive = prim;
        prim->obj.manual->setQueryFlags(GRAPH_OBJECTS_MASK);

        // set the pointer to the node
        Ogre::UserObjectBindings& binding = prim->obj.manual->getUserObjectBindings();
        binding.setUserAny(Ogre::Any(node.get()));
        result = node.get();
        mNodes.push_back(node);
        return true;
    }

    // get the Node from a movable object
    //
    GraphicNode* getNodeFromMove(Ogre::MovableObject* mo) {
        ASSERT(mo);
        const Ogre::Any& any = mo->getUserObjectBindings().getUserAny();
        if (any.isEmpty()) {
            // is not one of our objects
            return 0;
        }
        // return the object if is a selectable one
        return Ogre::any_cast<GraphicNode*>(any);
    }
    GraphicEdge* getEdgeFromMove(Ogre::MovableObject* mo) {
        ASSERT(mo);
        const Ogre::Any& any = mo->getUserObjectBindings().getUserAny();
        if (any.isEmpty()) {
            // is not one of our objects
            return 0;
        }
        // return the object if is a selectable one
        return Ogre::any_cast<GraphicEdge*>(any);
    }

    // create a new edge with two nodes
    bool createEdge(GraphicNode* n1, GraphicNode* n2, GraphicEdge*& result) {
        ASSERT(n1); ASSERT(n2);
        int i1 = getIndex(n1, mNodes);
        int i2 = getIndex(n2, mNodes);

        if (i1 < 0 || i2 < 0) {
            return false;
        }

        GraphicEdgePtr edge(new GraphicEdge);
        edge->n1 = n1;
        edge->n2 = n2;

        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

        // check here that the edge wasn't added twice to the same node
        n1->edges.insert(edge.get());
        n2->edges.insert(edge.get());

        core::Primitive* prim = pd.createLine(
            Ogre::Vector3(n1->position.x, n1->position.y, startingZ),
            Ogre::Vector3(n2->position.x, n2->position.y, startingZ),
            Ogre::ColourValue::Green);
        edge->primitive = prim;
        prim->obj.manual->setQueryFlags(0);

        // set the pointer to the node
        Ogre::UserObjectBindings& binding = prim->obj.manual->getUserObjectBindings();
        binding.setUserAny(Ogre::Any(edge.get()));
        result = edge.get();
        mEdges.push_back(edge);
        return true;
    }

    // remove a node (and all the edges)
    bool removeNode(GraphicNode* n1) {
        int index = getIndex(n1, mNodes);
        if (index < 0) {return false;}

        // we need to remove this and all the edges
        std::set<GraphicEdge*> toRemove = n1->edges;
        n1->edges.clear();
        for (std::set<GraphicEdge*>::iterator it = toRemove.begin(), end = toRemove.end();
                it != end; ++it) {
            removeEdge(*it);
        }
        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
        pd.deletePrimitive(n1->primitive);
        mNodes.erase(mNodes.begin()+index);

        return true;
    }

    // remove a edge.
    bool
    removeEdge(GraphicEdge* e) {
        int index = getIndex(e, mEdges);
        if (index < 0) return false;

        // advise the nodes
        e->n1->edges.erase(e);
        e->n2->edges.erase(e);

        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
        pd.deletePrimitive(e->primitive);

        mEdges.erase(mEdges.begin()+index);
        return true;
    }

    void
    removeAll(void)
    {
        while (!mNodes.empty()) {
            removeNode(mNodes.back().get());
        }
        while (!mEdges.empty()) {
            removeEdge(mEdges.back().get());
        }
    }

    template<typename T>
    int getIndex(T* e, std::vector<std::shared_ptr<T> >& v)
    {
        for (unsigned int i = 0; i < v.size(); ++i) {
            if (v[i].get() == e) {
                return i;
            }
        }
        return -1;
    }

    std::vector<GraphicNodePtr> mNodes;
    std::vector<GraphicEdgePtr> mEdges;

};

// ugly static var
static GraphicGraph sGraphicGraph;

// Command stuff
//
struct Command {
public:
    virtual ~Command(){};

    // execute the command, return true if it is possible to do an undo
    virtual bool execute(void) = 0;

    // undo the action, return true if it is possible to do a redo after (execute) again
    virtual bool undo(void) = 0;
};

struct CommandHandler {
    // execute a command
    //
    void execute(Command* cmd)
    {
        ASSERT(cmd);
        if (cmd->execute()) {
            mToUndo.push_front(cmd);
        } else {
            delete cmd;
        }
    }

    // undo last command executed
    //
    void undo(void)
    {
        if (mToUndo.empty()) {return;}
        Command* cmd = mToUndo.front();
        mToUndo.pop_front();
        if (cmd->undo()) {
            mToRedo.push_front(cmd);
        } else {
            delete cmd;
        }
    }

    // redo last command executed
    //
    void redo(void)
    {
        if (mToRedo.empty()) {return;}
        Command* cmd = mToRedo.front();
        mToRedo.pop_front();
        if (cmd->execute()){
            mToUndo.push_front(cmd);
        } else {
            delete cmd;
        }
    }

    // clear all
    //
    void clearAll(void)
    {
        while (!mToRedo.empty()) {
            delete mToRedo.front();
            mToRedo.pop_front();
        }
        while (!mToUndo.empty()) {
            delete mToUndo.front();
            mToUndo.pop_front();
        }
    }

private:
    std::deque<Command*> mToUndo;
    std::deque<Command*> mToRedo;
};

// ugly static var again!
static CommandHandler sCmdHandler;

// Define the possible commands here
//
struct CreateNodeCmd : Command {
    Ogre::Vector3 position;
    GraphicNode* node;

    CreateNodeCmd(const Ogre::Vector3& pos = Ogre::Vector3::ZERO) :
        position(pos)
    ,   node(0)
    {}

    bool execute(void)
    {
        ASSERT(node == 0);
        const bool ret = sGraphicGraph.createNode(position, node);
        return node != 0 && ret;
    }
    bool undo(void)
    {
        if (node == 0) {
            return false;
        }
        const bool ret = sGraphicGraph.removeNode(node);
        node = 0;
        return ret;
    }
};
struct CreateEdgeCmd : Command {
    GraphicNode* n1;
    GraphicNode* n2;
    GraphicEdge* edge;

    CreateEdgeCmd(GraphicNode* node1, GraphicNode* node2) :
        n1(node1)
    ,   n2(node2)
    ,   edge(0)
    {}

    bool execute(void)
    {
        if (n1 == 0 || n2 == 0) return false;

        ASSERT(edge == 0);
        const bool ret = sGraphicGraph.createEdge(n1, n2, edge);

        return ret && edge != 0;
    }
    bool undo(void)
    {
        if (edge == 0) return false;
        const bool ret = sGraphicGraph.removeEdge(edge);
        edge = 0;
        return ret;
    }
};

}


namespace tool {


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
            if (!mSelectionHelper.performRaycast(wp1, wp2, entry, LEVEL_OBJECTS_MASK)) {
                // no object intersects between this two elements
                wpgb.linkWaypoints(i, j);
            }
        }
    }

    // now we build the graph
    return wpgb.buildGraph(mGraph);
}

////////////////////////////////////////////////////////////////////////////////
void
PathfindingTool::buildGraphFromGraphic(void)
{
    // we will get all the nodes and
    std::map<GraphicNode*, int> indices;
    mWaypoints.resize(sGraphicGraph.mNodes.size());

    // configure nodes
    for (unsigned int i = 0, size = sGraphicGraph.mNodes.size(); i < size; ++i) {
        GraphicNode* gnode = sGraphicGraph.mNodes[i].get();
        ASSERT(gnode);
        indices[gnode] = i;
        mWaypoints[i] = gnode->position;
    }

    // now create the links
    gps::WayPointGraphBuilder wpgb;
    wpgb.setWaypointPosition(mWaypoints);
    core::BitMatrix<> adjacency;
    adjacency.build(mWaypoints.size(), mWaypoints.size());
    adjacency.reset();

    for (unsigned int i = 0, size = sGraphicGraph.mEdges.size(); i < size; ++i) {
        GraphicEdge* edge = sGraphicGraph.mEdges[i].get();
        ASSERT(edge);
        ASSERT(indices.find(edge->n1) != indices.end());
        ASSERT(indices.find(edge->n2) != indices.end());
        int n1 = indices[edge->n1];
        int n2 = indices[edge->n2];

        if (adjacency.get(n1, n2)) {
            // do nothing
            continue;
        }
        // no object intersects between this two elements
        wpgb.linkWaypoints(n1, n2);
        adjacency.set(n1, n2, true);
        adjacency.set(n2, n1, true);
    }

    // now we build the graph
    if (!wpgb.buildGraph(mGraph)) {
        debugERROR("Error building the Graphic graph!");
    }
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
        points[i].y = path.node[i].y;
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
PathfindingTool::buildingState(void)
{

    // if we press to build the graph we built it and then we remove all the
    // graphic information
    //
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_B)) {
        buildGraphFromGraphic();
        drawGraph(mGraph);
        mGraph.save("graph.wpg");
        mState = State::RunningPathfinding;
        mStatusBar.setText("State: RunningPathfinding", 0.015f);
        return;
    }

    // check for redo undo and return
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_LCONTROL)) {
        if (mInputHelper.isKeyReleased(input::KeyCode::KC_Z)) {
            if (mInputHelper.isKeyPressed(input::KeyCode::KC_LSHIFT)) {
                // redo
                sCmdHandler.redo();
            } else {
                sCmdHandler.undo();
            }
            // in any case return
            return;
        }
    }

    Ogre::RaySceneQueryResultEntry query;
    const bool lastRaycast = mSelectionHelper.getFirstRaycasted(query, GRAPH_OBJECTS_MASK);

    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left)) {
        // create a new node? or select one?

        Ogre::MovableObject* current = 0;
        if (lastRaycast) {
            // we select one
            ASSERT(query.movable);
            current = query.movable;
        } else {
            // we need to create a new one
            CreateNodeCmd* cmd = new CreateNodeCmd(mLastHitPos);
            sCmdHandler.execute(cmd);
            ASSERT(cmd->node->primitive);
            current = cmd->node->primitive->obj.manual;
        }

        // in any case we will change the colour of the current selected
        if (current != mLastMovableHit) {
            if (mLastMovableHit) {
                GraphicNode* node = sGraphicGraph.getNodeFromMove(mLastMovableHit);
                node->primitive->setColor(Ogre::ColourValue::Red);
            }
            if (current) {
                GraphicNode* cnode = sGraphicGraph.getNodeFromMove(current);
                cnode->primitive->setColor(Ogre::ColourValue::Blue);
            }
            mLastMovableHit = current;
        }
        return;
    }

    // check if we are trying to create an edge
    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Right) &&
        lastRaycast && mLastMovableHit != 0) {
        // if we are here then we press the right button, we could raycast
        // an element, and there is something selected before...
        ASSERT(query.movable);
        Ogre::MovableObject* current = query.movable;
        if (current == mLastMovableHit) {
            return; // nothing to do
        }
        // if they are different, we need to create an edge.
        GraphicNode* n1 = sGraphicGraph.getNodeFromMove(current);
        GraphicNode* n2 = sGraphicGraph.getNodeFromMove(mLastMovableHit);
        ASSERT(n1 != 0);    ASSERT(n2 != 0);
        CreateEdgeCmd* cmd = new CreateEdgeCmd(n1, n2);
        sCmdHandler.execute(cmd);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
PathfindingTool::runningPathfinderState(void)
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    // check for positions selections
    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left) &&
        mGraph.nodes().size > 0) {
        // check if is the first or the second position
        if (mSelectedPos.size() == 0) {
            // is the first one only
            mSelectedPos.push_back(core::Vector2(mLastHitPos.x, mLastHitPos.y));
        } else {
            mSelectedPos.push_back(core::Vector2(mLastHitPos.x, mLastHitPos.y));
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
            if (mVisualPath) {
                pd.deletePrimitive(mVisualPath);
            }
            mVisualPath = drawPath(path, pd.getFreshColour());
        }
    }
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

PathfindingTool::PathfindingTool() :
    core::AppTester(mTimeFrame)
,   mSatelliteCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mSelectionHelper(*mSceneMgr, *mCamera, mMouseCursor)
,   mVisualPath(0)
,   mState(State::BuildingGraph)
,   mLastMovableHit(0)
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

    mStatusBar.setPos(0,0.017f);
    mStatusBar.setColor(0, 1, 1, 1.f);
    mStatusBar.setText("State: BuildingGraph", 0.015f);
}

PathfindingTool::~PathfindingTool()
{

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
    mPathfinder.setGraph(&mGraph);
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
    if (mSelectionHelper.getPlaneIntersection(mLastHitPos)) {
        mMousePosText.setText("MapPosition: " + toString(mLastHitPos), 0.015f);
    }

    switch (mState) {
    case State::BuildingGraph:
        if (mInputHelper.isKeyReleased(input::KeyCode::KC_2)) {
            mState = State::RunningPathfinding;
            mStatusBar.setText("State: RunningPathfinding", 0.015f);
            return;
        }
        // else run the state
        buildingState();
        break;
    case State::RunningPathfinding:
        if (mInputHelper.isKeyReleased(input::KeyCode::KC_1)) {
            mState = State::BuildingGraph;
            mStatusBar.setText("State: BuildingGraph", 0.015f);
            return;
        }
        // else run the state
        runningPathfinderState();
        break;
    default:
        ASSERT(false);
    }

}

}
