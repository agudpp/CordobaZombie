/*
 * SelectionManager.cpp
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#include "SelectionManager.h"

#include <algorithm>

#include "GlobalObjects.h"
#include "DebugUtil.h"

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

SelectionManager::SelectionManager()
{

}

SelectionManager::~SelectionManager()
{

}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::addObject(SelectableObject *obj)
{
    if (exists(obj)) return;
    mSelectedObjects.push_back(obj);
    obj->objectSelected();
    mSignal(SelectionData(mSelectedObjects));
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::addObjects(const std::vector<SelectableObject *> &objects)
{
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        ASSERT(!exists(objects[i]));
        mSelectedObjects.push_back(objects[i]);
        objects[i]->objectSelected();
    }

    mSignal(SelectionData(mSelectedObjects));
}

void
SelectionManager::removeObject(SelectableObject *obj)
{
    ASSERT(exists(obj));
    removeElement(mSelectedObjects, obj);
    obj->objectUnselected();
    mSignal(SelectionData(mSelectedObjects));

}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::removeObjects(const std::vector<SelectableObject *> &objects)
{
    // this is slow, could be improved
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        removeElement(mSelectedObjects, objects[i]);
        objects[i]->objectUnselected();
    }
    mSignal(SelectionData(mSelectedObjects));
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionManager::exists(SelectableObject *obj)
{
    for(size_t i = 0, size = mSelectedObjects.size(); i < size; ++i){
        if (mSelectedObjects[i] == obj) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::removeAll(Type t)
{
    for(size_t i = 0, size = mSelectedObjects.size(); i < size; ++i){
        if (mSelectedObjects[i]->type() == t) {
            swapAndRemove(mSelectedObjects, i);
            mSelectedObjects[i]->objectUnselected();
            --i;
            --size;
        }
    }
    mSignal(SelectionData(mSelectedObjects));
}


void
SelectionManager::removeAll(void)
{
    Remover remover;
    std::for_each(mSelectedObjects.begin(), mSelectedObjects.end(), remover);

    mSelectedObjects.clear();
    mSignal(SelectionData(mSelectedObjects));
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
