/*
 * MouseSelectionHandler.cpp
 *
 *  Created on: 16/12/2012
 *      Author: agustin
 */

#include "MouseSelectionHandler.h"

#include <OgreSceneQuery.h>
#include <OgreMovableObject.h>

#include <boost/bind.hpp>

#include <DebugUtil.h>
#include <RaycastManager.h>
#include <GlobalObjects.h>
#include <SelectionManager.h>
#include <SelectableObject.h>


namespace {
// functions that returns the closest selection::SelectableObject
//
inline selection::SelectableObject *
performOgreRaycast(RaycastManager *rm, float x, float y, uint32 mask)
{
    Ogre::RaySceneQueryResult &result = rm->performOgreRay(
            x, y, mask);

    if (result.empty()) {
        // no objects picked
        return 0;
    }
    // get the associated SelectableObject
    ASSERT(result[0].worldFragment == 0);
    ASSERT(result[0].movable != 0);
    const Ogre::Any &anyObject = result[0].movable->getUserAny();
    ASSERT(!anyObject.isEmpty());
    const selection::SelectableObject *selObj =
            Ogre::any_cast<selection::SelectableObject *>(anyObject);
    ASSERT(selObj);
    return selObj;
}

}

namespace input {

////////////////////////////////////////////////////////////////////////////////
void
MouseSelectionHandler::performBoxSelection(void)
{
    ASSERT(false); // TODO
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MouseSelectionHandler::MouseSelectionHandler(RaycastManager *rm) :
        mRaycastMngr(rm),
        mLastMouseOverObj(0),
        mLastFrameClicked(false),
        mSelManager(selection::SelectionManager::getInstance()),
        mBoxSelection(false)
{
    ASSERT(rm);
    mConnection = selection::SelectionManager::getInstance().addCallback(
            boost::bind(&MouseSelectionHandler::selectionChanged, _1));
}

MouseSelectionHandler::~MouseSelectionHandler()
{
    mConnection.disconnect();
}


////////////////////////////////////////////////////////////////////////////////
void
MouseSelectionHandler::performRaycast(void)
{
    const bool leftPressed = InputMouse::isMouseDown(MouseButtonID::MB_Left);
    if (mLastMouseX == GLOBAL_CURSOR->getXRelativePos() &&
            mLastMouseY == GLOBAL_CURSOR->getYRelativePos()){
        if (mLastFrameClicked == false) {
            if (leftPressed){
                // we just press the button
                mLastFrameClicked = true;
            }
            // we have nothing else to do
            return;
        }
        // the button was pressed in the last frame, now check if we release
        // the button of the mouse
        if(leftPressed){
            // the button of the mouse is still pressed, so we should wait until
            // we release it... in this case we have nothing to do
            return;
        }

        // else the mouse was released, so we need to add the lastMouseOverObj
        // to the selection manager or check if is a boxSelection
        if (mBoxSelection) {
            ASSERT(!mBeginPoint.isReseted());
            performBoxSelection();
            mBoxSelection = false;
            mBeginPoint.reset();
            return;
        }
        // no box selection, check the lastMouseOverObj
        if (mLastMouseOverObj == 0) {
            // nothing to do
            return;
        }
        mSelManager.addObject(mLastMouseOverObj);
        return;
    }

    // the mouse position was changed, so we update the new one and do all the
    // associated logic
    mLastMouseX = GLOBAL_CURSOR->getXRelativePos();
    mLastMouseY = GLOBAL_CURSOR->getYRelativePos();

    // we need to know if we have to save the first position
    if (leftPressed){
        if (!mBoxSelection){
            mBeginPoint.x = mLastMouseX;
            mBeginPoint.y = mLastMouseY;
        }
        if (mLastFrameClicked){
            // we are making the box selection (using a box to select multiple
            // players or things, right now I think the only things that we can
            // select this way are the players itself).
            // TODO: draw box
            mBoxSelection = true;

            // we don't care about checking the objects that we can be "
            // mouseOvering...  So return
            return;
        }
    }

    // perform the query (using ogre query for now, probably we can improve
    // this using own raycast using collision manager...)
    mSelectedObjects.clear();

    uint32 mask = mSelState->getMask();
    selection::SelectableObject *selObj = performOgreRaycast(mRaycastMngr,
                                                             mLastMouseX,
                                                             mLastMouseY,
                                                             mask);
    // check if the object is already in the state "onMouseOver"
    if (selObj != mLastMouseOverObj) {
        // it is not...
        if (mLastMouseOverObj != 0) {
            mLastMouseOverObj->mouseExitObject();
        }
        if (selObj != 0) {
            selObj->mouseOverObject();
        }
        mLastMouseOverObj = selObj;
    }

}

////////////////////////////////////////////////////////////////////////////////
void
MouseSelectionHandler::selectionChanged(const selection::SelectionData &data)
{

}



}
