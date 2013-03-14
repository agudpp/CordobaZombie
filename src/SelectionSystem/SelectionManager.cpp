/*
 * SelectionManager.cpp
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#include "SelectionManager.h"

#include <algorithm>

#include "GlobalObjects.h"


namespace {
// Auxiliar function used to swap the last element
inline void
swapAndRemove(std::vector<selection::SelectableObject *> &vec, size_t pos)
{
    vec[pos] = vec[vec.size()-1];
    vec.pop_back();
}

inline void
removeElement(std::vector<selection::SelectableObject *> &vec,
              const selection::SelectableObject *obj)
{
    for(size_t i = 0, size = vec.size(); i < size; ++i){
        if (vec[i] == obj) {
            swapAndRemove(vec, i);
            return;
        }
    }
}

struct Remover {
    void operator()(selection::SelectableObject *o) {o->objectUnselected();}
};


}


namespace selection {

SelectionManager::SelectionManager() :
    mDirty(false)
{

}

SelectionManager::~SelectionManager()
{

}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::select(SelectableObject *obj)
{
    ASSERT(obj);
    if (isSelected(obj)){
        return;
    }
    mDirty = true;
    mSelectedObjects.push_back(obj);
    obj->objectSelected();
    mLastSelection.push_back(obj);

    // if this object was selected after any unselection we need to clear the
    // object from unselected vector
    removeElement(mUnselected, obj);
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::select(const std::vector<SelectableObject *> &objects)
{
    mDirty = true;
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        SelectableObject *obj = objects[i];
        ASSERT(obj);
        if (isSelected(obj)){
            continue;
        }
        mSelectedObjects.push_back(objects[i]);
        objects[i]->objectSelected();
        mLastSelection.push_back(obj);

        // just in case remove if from the unselected vector if
        removeElement(mUnselected, obj);
    }
}

void
SelectionManager::unselect(SelectableObject *obj)
{
    ASSERT(obj);
    mDirty = true;
    if (!isSelected(obj)){
        return;
    }
    swapAndRemove(mSelectedObjects, obj->mIndex);
    obj->objectUnselected();

    // just in case remove it from the last selection vec
    removeElement(mLastSelection, obj);

    mUnselected.push_back(obj);
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::unselect(const std::vector<SelectableObject *> &objects)
{
    mDirty = true;

    // this is slow, could be improved
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        SelectableObject *obj = objects[i];
        ASSERT(obj);
        if (!isSelected(obj)) {
            continue;
        }
        swapAndRemove(mSelectedObjects, obj->mIndex);
        obj->objectUnselected();
        mUnselected.push_back(obj);

        // just in case remove it from the last selection vec
        removeElement(mLastSelection, obj);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::unselectAll(Type t)
{
    mDirty = true;
    mUnselected.clear();
    for(size_t i = 0, size = mSelectedObjects.size(); i < size; ++i){
        if (mSelectedObjects[i]->type() == t) {
            SelectableObject *obj = mSelectedObjects[i];
            mUnselected.push_back(obj);
            swapAndRemove(mSelectedObjects, i);
            obj->objectUnselected();
            --i;
            --size;
            // just in case remove it from the last selection vec
            removeElement(mLastSelection, obj);
        }
    }
}


void
SelectionManager::unselectAll(void)
{
    mDirty = true;
    Remover remover;
    std::for_each(mSelectedObjects.begin(), mSelectedObjects.end(), remover);

    mUnselected = mSelectedObjects;
    mLastSelection.clear();
    mSelectedObjects.clear();
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::getObjects(Type t, ObjectsVec &result) const
{
    for(size_t i = 0, size = mSelectedObjects.size(); i < size; ++i){
        if (mSelectedObjects[i]->type() == t){
            result.push_back(mSelectedObjects[i]);
        }
    }
}


}
