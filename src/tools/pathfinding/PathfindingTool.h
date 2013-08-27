/*
 * PathfindingTool.h
 *
 *  Created on: 15/08/2013
 *      Author: agustin
 */

#ifndef PathfindingTool_H_
#define PathfindingTool_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>
#include <OgrePlane.h>

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <utils/SatelliteCamera.h>
#include <utils/SelectionHelper.h>
#include <input/InputHelper.h>
#include <pathfinding/WayPointGraph.h>
#include <pathfinding/WPPathfinder.h>
#include <pathfinding/WayPointDefines.h>
#include <debug/PrimitiveDrawer.h>
#include <cursor/MouseCursor.h>
#include <types/StackVector.h>



// The xml file for this tool
// <PathfindingTool scene="file.scene" waypoints="waypoints.txt" />
//
#define PATHFINDING_TOOL_XML_FILE    "PathfindingTool.xml"


namespace tool {

class PathfindingTool : public core::AppTester
{
    enum State {
        BuildingGraph = 0,
        RunningPathfinding,
    };
public:
    PathfindingTool();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~PathfindingTool();

private:

    // @brief Read the xml file and retrieve the scene filename and the waypoints
    //        filename.
    // @param xmlFName  The xml filename
    // @param scene     The .scene filename
    // @param waypoints The waypoints filename
    // @return true on success | false otherwise
    //
    bool
    parseXML(const std::string& xmlFName,
             std::string& scene,
             std::string& waypoints) const;

    // @brief Load the .scene
    // @param scene     The scene filename to load.
    // @return true on success | false otherwise
    //
    bool
    loadScene(const std::string& scene);

    // @brief Load the waypoints and show them into the the level
    // @param file      The filename containing the waypoints information
    // @return true on success | false otherwise
    //
    bool
    loadWaypoints(const std::string& file);

    // @brief Build the WayPointGraph using the already parsed waypoints.
    // @return true on success | false otherwise
    //
    bool
    buildGraph(void);

    // @brief Build the graph using the GraphicGraph already created.
    //        Once you call this function the GraphicGraph is deleted
    //        and all the new graph is set
    //
    void
    buildGraphFromGraphic(void);

    // @brief Draw a graph into the scene
    // @param graph     The graph we want to draw
    //
    void
    drawGraph(const gps::WayPointGraph& graph,
              const Ogre::ColourValue& color = Ogre::ColourValue::Blue);

    // @brief Draw a new path into the scene and returning the PrimitiveDraw
    //        pointer.
    // @param path      The path to draw
    // @param color     The color we want to use
    //
    core::Primitive*
    drawPath(const gps::WayPointPath& path, const Ogre::ColourValue& color);

    // @brief Get a position in the map using the current mouse cursor position
    // @param pos   The resulting position in the map.
    // @return true if we could get a point, false otherwise
    //
    bool
    pickPoint(Ogre::Vector3& post);

    // @brief State methods
    //
    void
    buildingState(void);
    void
    runningPathfinderState(void);


    void
    handleCameraInput(void);

private:
    float mTimeFrame;
    ui::MouseCursor mMouseCursor;
    SatelliteCamera mSatelliteCamera;
    input::InputHelper mInputHelper;
    SelectionHelper mSelectionHelper;
    std::vector<gps::Vertex> mWaypoints;
    core::OgreText mMousePosText;
    core::OgreText mStatusBar;

    // graph data
    gps::WayPointGraph mGraph;
    gps::WPPathfinder mPathfinder;
    core::Primitive* mVisualPath;
    core::StackVector<core::Vector2, 2> mSelectedPos;

    // state
    State mState;
    Ogre::MovableObject* mLastMovableHit;
    Ogre::Vector3 mLastHitPos;
};

}

#endif /* PathfindingTool_H_ */
