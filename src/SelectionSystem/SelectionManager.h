/*
 * SelectionManager.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTIONMANAGER_H_
#define SELECTIONMANAGER_H_

#include <vector>
#include <set>
#include <boost/signals.hpp>

#include <DebugUtil.h>

#include "SelectableObject.h"
#include "SelectionType.h"
#include "SelectionData.h"

namespace selection {

class SelectionManager {
public:

    // Define the signals / callbacks types
    //
    typedef boost::signal<void (const SelectionData &)>   SelectionSignal;
    typedef boost::signals::connection                    Connection;
    typedef std::vector<SelectableObject *>               ObjectsVec;

public:
    // Returns the instance
    //
    static SelectionManager &getInstance(void)
    {
        static SelectionManager instance;
        return instance;
    }

    // Destructor
    //
    ~SelectionManager();

    /**
     * Add/Unselect a selectable object
     * These functions will call the objectSelected / Unselected functions
     * If the object is already selected (unselected) then this function does
     * nothing. Otherwise it will call also the callbacks
     * @Note: This class is not the owner of the memory
     */
    void select(SelectableObject *obj);
    void select(const std::vector<SelectableObject *> &objects);
    void unselect(SelectableObject *obj);
    void unselect(const std::vector<SelectableObject *> &objects);

    /**
     * Check if an object already exists
     */
    inline bool isSelected(SelectableObject *obj);

    /**
     * Unselect by Type (removes all the objects with a certain type)
     * @Note: This class is not the owner of the memory
     */
    void unselectAll(Type t);
    void unselectAll(void);

    /**
     * Get all the objects associated with a type
     */
    void getObjects(Type t, ObjectsVec &result) const;

    /**
     * Returns all the objects
     */
    inline const ObjectsVec &getObjects(void) const;

    /**
     * Add/remove selection change callbacks
     */
    inline Connection addCallback(const SelectionSignal::slot_type &cb);
    inline void removeCallback(const Connection &cb);

    /**
     * @brief This function needs to be called every frame to call all the
     *        callbacks we have associated to this class. This way we can
     *        package the events all in one
     */
    inline void executeCallbacks(void);


private:
    // Avoid construction / copying the class
    //
    SelectionManager();
    SelectionManager(const SelectionManager &);
    SelectionManager &operator=(const SelectionManager &);

private:
    SelectionSignal mSignal;
    ObjectsVec mSelectedObjects;
    ObjectsVec mUnselected;
    ObjectsVec mLastSelection;
    bool mDirty;
};


// Inline implementations
//

inline bool
SelectionManager::isSelected(SelectableObject *obj)
{
    return (obj != 0 ) && (obj->mIndex < mSelectedObjects.size()) && (
            mSelectedObjects[obj->mIndex] == obj);
}

inline SelectionManager::Connection
SelectionManager::addCallback(const SelectionSignal::slot_type &cb)
{
    return mSignal.connect(cb);
}
inline void
SelectionManager::removeCallback(const Connection &cb)
{
    mSignal.disconnect(cb);
}

////////////////////////////////////////////////////////////////////////////////
const SelectionManager::ObjectsVec &
SelectionManager::getObjects(void) const
{
    return mSelectedObjects;
}

////////////////////////////////////////////////////////////////////////////////
inline void
SelectionManager::executeCallbacks(void)
{
    if (!mDirty){
        return;
    }
    mDirty = false;
    mSignal(SelectionData(mSelectedObjects, mLastSelection, mUnselected));
    mLastSelection.clear();
    mUnselected.clear();
}

}

#endif /* SELECTIONMANAGER_H_ */
