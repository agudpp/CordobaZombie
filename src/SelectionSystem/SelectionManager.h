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
     * Add/Remove a selectable object
     * These functions will call the objectSelected / Unselected functions
     * @Note: This class is not the owner of the memory
     */
    void addObject(SelectableObject *obj);
    void addObjects(const std::vector<SelectableObject *> &objects);
    void removeObject(SelectableObject *obj);
    void removeObjects(const std::vector<SelectableObject *> &objects);

    /**
     * Check if an object already exists
     */
    bool exists(SelectableObject *obj);

    /**
     * Remove by Type (removes all the objects with a certain type)
     * @Note: This class is not the owner of the memory
     */
    void removeAll(Type t);
    void removeAll(void);

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


private:
    // Avoid construction / copying the class
    //
    SelectionManager();
    SelectionManager(const SelectionManager &);
    SelectionManager &operator=(const SelectionManager &);

private:
    SelectionSignal mSignal;
    ObjectsVec mSelectedObjects;
};

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

}

#endif /* SELECTIONMANAGER_H_ */
