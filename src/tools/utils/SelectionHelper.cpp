/*
 * SelectionHelper.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: agustin
 */

#include "SelectionHelper.h"

#include <OgreUserObjectBindings.h>

#include <types/basics.h>
#include <input/InputMouse.h>

namespace tool {

////////////////////////////////////////////////////////////////////////////////
SelectableObject::SelectableObject(Ogre::SceneNode* node) :
    mNode(node)
{
    if (mNode != 0) {
        Ogre::UserObjectBindings& binding = mNode->getUserObjectBindings();
        binding.setUserAny(Ogre::Any(this));
    }
}

void
SelectableObject::setSceneNode(Ogre::SceneNode* node)
{
    ASSERT(node);
    mNode = node;
    Ogre::UserObjectBindings& binding = mNode->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(this));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::performRaycast()
{
    ASSERT(mRaySceneQuery);

    // configure the ray
    mMouseRay = mCamera.getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                               mMouseCursor.getYRelativePos());
    mRaySceneQuery->setRay(mMouseRay);
    mRaySceneQuery->setQueryMask(~0); // all
    mRaySceneQuery->setSortByDistance(true, 1); // the first one

    // execute the ray and get the results
    Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
    if (result.empty()) {
        // nothing to return
        return 0;
    }
    ASSERT(result.size() == 1); // we want only one

    Ogre::RaySceneQueryResultEntry& entry = result.back();
    if (entry.movable == 0) {
        debugWARNING("We pick a worldfragment object... that is ok?\n");
        return 0;
    }
    Ogre::SceneNode* node = entry.movable->getParentSceneNode();
    if (node == 0) {
        debugERROR("Some movable object without scene node associated!!\n");
        return 0;
    }

    const Ogre::Any& any = node->getUserObjectBindings().getUserAny();
    if (any.isEmpty()) {
        // is not one of our objects
        return 0;
    }

    // return the object if is a selectable one
    return Ogre::any_cast<SelectableObject*>(any);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

SelectionHelper::SelectionHelper(Ogre::SceneManager& sm,
                                 Ogre::Camera& camera,
                                 const ui::MouseCursor& mouseCursor) :
    mSceneMngr(sm)
,   mCamera(camera)
,   mMouseCursor(mouseCursor)
,   mLastSelection(0)
,   mLastRaycasted(0)
,   mRaySceneQuery(0)
{
    mRaySceneQuery = mSceneMngr.createRayQuery(mMouseRay,~0);
}

SelectionHelper::~SelectionHelper()
{
    mSceneMngr.destroyQuery(mRaySceneQuery);
}

////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::update(void)
{
    // get the current position
    const Ogre::Vector2 currentPos(mMouseCursor.getXRelativePos(),
                                   mMouseCursor.getYRelativePos());
    const bool leftButtonPressed = input::Mouse::isMouseDown(input::MouseButtonID::MB_Left);

    // check if we are in the same position (and assuming that nothing is
    // moving)
    if (currentPos == mLastMousePos) {
        // check if we click
        if (leftButtonPressed) {
            if (mLastRaycasted == 0) {
                // no element raycasted, unselect all and return
                unselectAll();
            } else {
                // some object was raycasted

                // we are only allowing to select one object per time using the
                // mouse and this way (in the update).
                // We can select multiple objects outside this class getting the
                // last selectable object
                if (!isSelected(mLastRaycasted)) {
                    unselectAll();
                    select(mLastRaycasted, SelectType::LeftButton);
                }
            }
        }
        // do nothing, no new object intersected
        return mLastRaycasted;
    }

    // update the new position
    mLastMousePos = currentPos;

    // check if new object is raycasted
    SelectableObject* newObjectRaycasted = performRaycast();

    if (newObjectRaycasted != 0) {
        // no new object raycasted, if we had object raycasted before we need
        // to advise it
        if (newObjectRaycasted == mLastRaycasted) {
            // is the same than before, check if we have need to select it
            if (leftButtonPressed) {
                // check if we have to select the new raycasted object
                if (!isSelected(newObjectRaycasted)) {
                    unselectAll();
                    // yes we need
                    select(newObjectRaycasted, SelectType::LeftButton);
                }
            }
            return mLastRaycasted;
        }

        // the new raycasted object is a new one, trigger the event
        newObjectRaycasted->mouseOver();

        // check if we have to advise to the old one
        if (mLastRaycasted != 0) {
            mLastRaycasted->mouseExit();
        }
    } else {
        // we have no object raycasted... check if we had one before
        if (mLastRaycasted != 0) {
            mLastRaycasted->mouseExit();
        }

        // if we press the button we need to unselect everything
        if (leftButtonPressed) {
            unselectAll();
        }
    }

    // in any case we should update the mLastRaycasted with the new one
    return mLastRaycasted = newObjectRaycasted;
}

////////////////////////////////////////////////////////////////////////////////
SelectableObject*
SelectionHelper::getLastRaycasted(void)
{
    return mLastRaycasted;
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::getSelected(std::vector<SelectableObject*>& selected)
{
    selected = mSelectedObjects;
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::select(SelectableObject* object, SelectType type)
{
    ASSERT(object);

    // check if the object is already selected
    if (isSelected(object)) {
        // do nothing
        return;
    }

    if (!object->objectSelected(type)) {
        // we should not track this object.
        return;
    }
    mLastSelection = object;

    // else add it to the list
    object->id = mSelectedObjects.size();
    mSelectedObjects.push_back(object);
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::unselect(SelectableObject* object)
{
    ASSERT(object);
    ASSERT(object->id < mSelectedObjects.size());
    ASSERT(mSelectedObjects[object->id] == object);
    object->objectUnselected();

    // swap with last
    mSelectedObjects[object->id] = mSelectedObjects.back();
    mSelectedObjects[object->id]->id = object->id;
    mSelectedObjects.pop_back();

    if (mLastSelection == object) {
        mLastSelection = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::unselectAll(void)
{
    for (core::size_t i = 0, size = mSelectedObjects.size(); i < size; ++i) {
        mSelectedObjects[i]->objectUnselected();
    }
    mLastSelection = 0;
    mSelectedObjects.clear();
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::setBaseIntersectionPlane(const Ogre::Plane& plane)
{
    mPlane = plane;
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionHelper::getPlaneIntersection(const Ogre::Vector2& mousePos, Ogre::Vector3& intPos)
{
    // set up the ray
    mMouseRay = mCamera.getCameraToViewportRay(mousePos.x, mousePos.y);

    // check if the ray intersects our plane
    // intersects() will return whether it intersects or not (the bool value) and
    // what distance (the Real value) along the ray the intersection is
    std::pair<bool, Ogre::Real> result = mMouseRay.intersects(mPlane);

    if (result.first)
    {
        // if the ray intersect the plane, we have a distance value
        // telling us how far from the ray origin the intersection occurred.
        // the last thing we need is the point of the intersection.
        // Ray provides us getPoint() function which returns a point
        // along the ray, supplying it with a distance value.

        // get the point where the intersection is
        Ogre::Vector3 v = mMouseRay.getPoint(result.second);
        intPos.x = v.x;
        intPos.y = v.z;
        return true;
    }

    return false;
}

} /* namespace tool */
