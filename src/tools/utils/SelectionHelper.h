/*
 * SelectionHelper.h
 *
 *  Created on: Jun 30, 2013
 *      Author: agustin
 */

#ifndef SELECTIONHELPER_H_
#define SELECTIONHELPER_H_

#include <vector>

#include <OgreVector2.h>
#include <OgreSceneNode.h>
#include <OgreRay.h>
#include <OgrePlane.h>
#include <OgreSceneQuery.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>

#include <debug/DebugUtil.h>
#include <cursor/MouseCursor.h>


namespace tool {

// Define the enum for the events when the objects are clicked
//
enum SelectType {
    RightButton = 0,
    LeftButton,
};

// Forward
//
class SelectionHelper;

// Define the interface for the selection object classes
//
class SelectableObject {
public:
    // @brief Constructor should take a SceneNode to be configured the userDefined
    //        object pointing to this object. If a scene node hasn't userDefined
    //        object will be discarded.
    //
    SelectableObject(Ogre::SceneNode* node = 0);
    virtual
    ~SelectableObject(){}

    // @brief Set the SceneNode used by this object
    // @param node  The node to be associated to this object
    //
    void
    setSceneNode(Ogre::SceneNode* node);

    // @brief Method called when we pass the mouse over the object
    //
    virtual void
    mouseOver(void) = 0;

    // @brief Method called when the mouse is out of the object
    //
    virtual void
    mouseExit(void) = 0;

    // @brief Method called when the object is selected
    // @param type  Determines the selection type (button of the mouse)
    // @return true if the object should be selected or false if not. if we
    //         return false the object will be not took into account as if was
    //         selected (we will never call objectUnselected()). If we return true
    //         we will track it as a selected object.
    //
    virtual bool
    objectSelected(SelectType type) = 0;

    // @brief Method called when the object is unselected
    //
    virtual void
    objectUnselected(void) = 0;

    // @brief Returns the associated sceneNode
    //
    inline Ogre::SceneNode*
    getSceneNode(void) {return mNode;}

private:
    Ogre::SceneNode* mNode;

    // SelectionHelper usage only
    friend class SelectionHelper;
    unsigned int id;
};

class SelectionHelper
{
public:
    // @brief We need the SceneManager reference that we will use and also the
    //        camera used to perform the raycasts as well as the MouseCursor
    //
    SelectionHelper(Ogre::SceneManager& sm,
                    Ogre::Camera& camera,
                    const ui::MouseCursor& mouseCursor);
    ~SelectionHelper();

    // @brief This method will execute a raycast and will update the internal
    //        logic (call the elements that are not being selected anymore, the
    //        elements who are being pointed by the mouse, etc). And we also
    //        return the new intersected object just in case is needed.
    // @return the raycasted object (the closest one). or 0 if no object was
    //         found
    // @note We will use input::Mouse to check for mouse press
    //
    SelectableObject*
    update(void);

    // @brief Return the last raycasted object.
    //
    SelectableObject*
    getLastRaycasted(void);

    // @brief Get all the selected objects
    //
    void
    getSelected(std::vector<SelectableObject*>& selected);

    // @brief Select a new object (this will call the objectClicked method with,
    //        the associated type and call the objectUnselected() when necessary).
    // @param object    The selectable object to be added as selected.
    // @param type      The selection type to be used when we call objectClicked();
    //
    void
    select(SelectableObject* object, SelectType type = SelectType::LeftButton);

    // @brief Check if an object is already selected
    // @param object    The object we want to check
    //
    inline bool
    isSelected(SelectableObject* object) const;

    // @brief Unselect a specific object (this method WILL call the objectUnselected()
    // @param object    The object to be unselected
    // @note The object should be tracked by the SelectionHelper and should be
    //       already selected.
    //
    void
    unselect(SelectableObject* object);

    // @brief Usnelect all the elements
    //
    void
    unselectAll(void);

    // @brief Configure the plane for which we want to use as the base for
    //        intersections to create objects or positionate them.
    // @param plane     The plane we will use
    //
    void
    setBaseIntersectionPlane(const Ogre::Plane& plane);

    // @brief Get the interesction point in the plane for a given raycast
    // @param mousePos  The position of the mouse to execute the raycast (this
    //                  position should be the relative position of the mouse)
    // @param intPos    The intersection position from the raycast into the plane
    // @return true if we get an intersection and intPos contains the intersection
    //         point. False if we didn't intersect the plane.
    //
    bool
    getPlaneIntersection(const Ogre::Vector2& mousePos, Ogre::Vector3& intPos);

    // TODO: add signals handling stuff here to emit events when new selection
    //       unselection occurs.
    //

private:

    // @brief Execute a raycast and get the associated object. If no object
    //        is raycasted then return 0. We will use the MouseCursor position
    // @return the raycasted object | null
    //
    SelectableObject*
    performRaycast(void);

private:
    Ogre::SceneManager& mSceneMngr;
    Ogre::Camera& mCamera;
    const ui::MouseCursor& mMouseCursor;
    SelectableObject* mLastSelection;
    SelectableObject* mLastRaycasted;
    std::vector<SelectableObject*> mSelectedObjects;
    Ogre::Plane mPlane;
    Ogre::Ray mMouseRay;
    Ogre::RaySceneQuery *mRaySceneQuery;
    Ogre::Vector2 mLastMousePos;
};


////////////////////////////////////////////////////////////////////////////////

// Inline stuff
//
inline bool
SelectionHelper::isSelected(SelectableObject* object) const
{
    ASSERT(object);
    return object->id < mSelectedObjects.size() &&
        mSelectedObjects[object->id] == object;
}

} /* namespace tool */
#endif /* SELECTIONHELPER_H_ */
