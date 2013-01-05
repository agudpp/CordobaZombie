/*
 * MouseSelectionHandler.h
 *
 *  Created on: 16/12/2012
 *      Author: agustin
 */

#ifndef MOUSESELECTIONHANDLER_H_
#define MOUSESELECTIONHANDLER_H_

#include <boost/signals.hpp>
#include <vector>

#include "MouseSelectionState.h"

namespace selection {
class SelectionData;
class SelectableObject;
class SelectionManager;
}

class RaycastManager;


namespace input {

class MouseSelectionHandler {
    typedef boost::signals::connection Connection;
    typedef std::vector<selection::SelectableObject *> SelectedObjsVec;

    struct ScreenPoint {
        ScreenPoint(){reset();}
        void reset(void) {x = -1; y = -1;}
        bool isReseted(void) const {return (x != -1.0f) && (y != -1.0f);}

        float x;
        float y;
    };
public:
    MouseSelectionHandler(RaycastManager *rm);
    ~MouseSelectionHandler();

    /**
     * Perform the raycast using mouse position
     */
    void performRaycast(void);

    /**
     * Receive the callback events from the selection manager
     */
    void selectionChanged(const selection::SelectionData &data);

private:
    /**
     * Perform box selection
     */
    void performBoxSelection(void);
private:
    RaycastManager *mRaycastMngr;
    float mLastMouseX;
    float mLastMouseY;
    Connection mConnection;
    SelectedObjsVec mSelectedObjects;
    MouseSelectionState *mSelState;
    selection::SelectableObject *mLastMouseOverObj;
    selection::SelectionManager &mSelManager;
    bool mLastFrameClicked;
    bool mBoxSelection;
    ScreenPoint mBeginPoint;
};

}

#endif /* MOUSESELECTIONHANDLER_H_ */
