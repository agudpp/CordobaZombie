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

SelectionManager::SelectionManager()
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
    mSelectedObjects.push_back(obj);
    obj->objectSelected();
    mAuxVec.clear();
    mAuxVec.push_back(obj);
    mSignal(SelectionData(mAuxVec, true));
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::select(const std::vector<SelectableObject *> &objects)
{
    mAuxVec.clear();
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        SelectableObject *obj = objects[i];
        ASSERT(obj);
        if (isSelected(obj)){
            continue;
        }
        mSelectedObjects.push_back(objects[i]);
        objects[i]->objectSelected();
        mAuxVec.push_back(obj);
    }

    mSignal(SelectionData(mAuxVec, true));
}

void
SelectionManager::unselect(SelectableObject *obj)
{
    ASSERT(obj);
    if (!isSelected(obj)){
        return;
    }
    swapAndRemove(mSelectedObjects, obj->mIndex);
    obj->objectUnselected();
    mAuxVec.clear();
    mAuxVec.push_back(obj);
    mSignal(SelectionData(mAuxVec, false));

}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::unselect(const std::vector<SelectableObject *> &objects)
{
    mAuxVec.clear();
    // this is slow, could be improved
    for(size_t i = 0, numObjs = objects.size(); i < numObjs; ++i){
        SelectableObject *obj = objects[i];
        ASSERT(obj);
        if (!isSelected(obj)) {
            continue;
        }
        swapAndRemove(mSelectedObjects, obj->mIndex);
        obj->objectUnselected();
        mAuxVec.push_back(obj);
    }
    mSignal(SelectionData(mAuxVec, false));
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionManager::unselectAll(Type t)
{
    mAuxVec.clear();
    for(size_t i = 0, size = mSelectedObjects.size(); i < size; ++i){
        if (mSelectedObjects[i]->type() == t) {
            mAuxVec.push_back(mSelectedObjects[i]);
            swapAndRemove(mSelectedObjects, i);
            mSelectedObjects[i]->objectUnselected();
            --i;
            --size;

        }
    }
    mSignal(SelectionData(mAuxVec, false));
}


void
SelectionManager::unselectAll(void)
{
    Remover remover;
    std::for_each(mSelectedObjects.begin(), mSelectedObjects.end(), remover);

    mSignal(SelectionData(mSelectedObjects, false));
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
