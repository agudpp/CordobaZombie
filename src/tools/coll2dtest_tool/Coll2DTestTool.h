/*
 * Coll2DTestTool.h
 *
 *  Created on: 15/08/2013
 *      Author: agustin
 */

#ifndef Coll2DTestTool_H_
#define Coll2DTestTool_H_

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
#include <debug/PrimitiveDrawer.h>
#include <cursor/MouseCursor.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollPreciseInfo.h>
#include <collisions/CollObject.h>
#include <collisions/CollDefines.h>
#include <collisions/CollPreciseInfoBuilder.h>


// The xml file for this tool
// <Coll2DTestTool scene="file.scene" />
//
#define COLLTESTTOOL_TOOL_XML_FILE    "Coll2DTestTool.xml"


namespace tool {

class Coll2DTestTool : public core::AppTester
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
    Coll2DTestTool();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~Coll2DTestTool();

private:

    // @brief Read the xml file and retrieve the scene filename.
    // @param xmlFName  The xml filename
    // @param scene     The .scene filename
    // @return true on success | false otherwise
    //
    bool
    parseXML(const std::string& xmlFName, std::string& scene) const;

    // @brief Load the .scene
    // @param scene     The scene filename to load.
    // @return true on success | false otherwise
    //
    bool
    loadScene(const std::string& scene);

    // @brief Build all the collision objects from the loaded meshes in the
    //        mCollObjectsNode.
    // @return true on success | false on error
    bool
    createCollObjects(const std::string& scene);

    // @brief Handle the player movement
    //
    void
    handlePlayerInput(void);

    // @brief Check for the collisions of the player
    //
    void
    handlePlayerCollisions(void);

    void
    handleCameraInput(void);

private:
    float mTimeFrame;
    ui::MouseCursor mMouseCursor;
    SatelliteCamera mSatelliteCamera;
    input::InputHelper mInputHelper;
    SelectionHelper mSelectionHelper;
    core::OgreText mMousePosText;
    core::OgreText mPlayerPosText;
    Ogre::SceneNode* mCollObjectsNode;
    std::vector<TmpPrimitiveColor> mPrimitives;
    coll::CollisionHandler mCollHandler;
    coll::CollObject* mPlayer;
    core::Primitive* mPlayerPrim;
    coll::QueryResultVec mLastColls;
};

}

#endif /* Coll2DTestTool_H_ */
