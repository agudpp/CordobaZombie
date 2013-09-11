/*
 * ZombieMovementTool.h
 *
 *  Created on: 15/08/2013
 *      Author: agustin
 */

#ifndef ZombieMovementTool_H_
#define ZombieMovementTool_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>
#include <OgrePlane.h>

#include <vector>
#include <string>

// general
#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <debug/PrimitiveDrawer.h>
#include <cursor/MouseCursor.h>
#include <types/StackVector.h>
#include <input/InputHelper.h>
#include <types/basics.h>

// camera
#include <utils/SatelliteCamera.h>

// pathfinding
#include <pathfinding/WayPointGraph.h>
#include <pathfinding/WPPathfinder.h>
#include <pathfinding/WayPointDefines.h>

// colls
#include <collisions/CollisionHandler.h>
#include <collisions/CollPreciseInfo.h>
#include <collisions/CollObject.h>
#include <collisions/CollDefines.h>
#include <collisions/CollPreciseInfoBuilder.h>

// game specific
#include <zombie_unit/ZombieUnit.h>
#include <static_data_loader/StaticDataLoader.h>
#include <global_data/GlobalData.h>



// The xml file for this tool
// <ZombieMovementTool scene="" collScene="colisiones.scene" graphFile=""/>
//
#define ZOMBIEMOVEMENET_TOOL_XML_FILE    "ZombieMovementTool.xml"


namespace tool {

class ZombieMovementTool : public core::AppTester
{
    struct TmpPrimitiveColor {
        core::Primitive* primitive;
        Ogre::ColourValue realColor;
        coll::CollObject* collObj;

        TmpPrimitiveColor(core::Primitive* p, Ogre::ColourValue color) :
            primitive(p)
        ,   realColor(color)
        ,   collObj(0)
        {
            ASSERT(p);
            p->setColor(color);
        }

        // @brief set a temp colour
        inline void setTmpColor(Ogre::ColourValue col) {
            primitive->setColor(col);
        }

        // @brief reset the real color for this primitive
        inline void resetColor(void) {
            primitive->setColor(realColor);
        }
    };

public:
    ZombieMovementTool();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~ZombieMovementTool();

private:

    // @brief Read the xml file and retrieve the scene filename and the waypoints
    //        filename.
    // @param xmlFName  The xml filename
    // @param scene     The .scene filename
    // @param graphFile The graph filename
    // @param collScene The collision filename scene
    // @return true on success | false otherwise
    //
    bool
    parseXML(const std::string& xmlFName,
             std::string& scene,
             std::string& graphFile,
             std::string& collScene) const;

    // @brief Load the .scene
    // @param scene     The scene filename to load.
    // @return true on success | false otherwise
    //
    bool
    loadScene(const std::string& scene);

    // @brief Load the graph and show them into the the level
    // @param file      The filename containing the graph information
    // @return true on success | false otherwise
    //
    bool
    loadGraph(const std::string& file);

    // @brief Draw the path
    //
    void
    drawGraph(const gps::WayPointGraph& graph,
              const Ogre::ColourValue& color = Ogre::ColourValue::Red);

    // @brief Create coll objects from a scene
    //
    bool
    createCollObjects(const std::string& scene);

    // @brief Load zombies
    //
    void
    createZombies(void);

    void
    handleCameraInput(void);

private:
    // Global data loader
    cz::StaticDataLoader mData;

    float& mTimeFrame;
    ui::MouseCursor mMouseCursor;
    SatelliteCamera mSatelliteCamera;
    input::InputHelper mInputHelper;
    core::OgreText mMousePosText;
    core::OgreText mStatusBar;

    // graph data
    gps::WayPointGraph mGraph;
    gps::WPPathfinder& mPathfinder;

    // coll
    Ogre::SceneNode* mCollObjectsNode;
    std::vector<TmpPrimitiveColor> mPrimitives;
    coll::CollisionHandler& mCollHandler;

    core::StackVector<cz::ZombieUnit*, 100> mZombies;
    cz::ZombieUnit mZombie;
};

}

#endif /* ZombieMovementTool_H_ */
