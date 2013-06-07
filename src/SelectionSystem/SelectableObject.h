/*
 * SelectableObject.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTABLEOBJECT_H_
#define SELECTABLEOBJECT_H_

#include <OgreMovableObject.h>

#include "SelectionType.h"
#include "SelectionData.h"

namespace selection {


// forward declaration
class SelectionManager;

class SelectableObject {
public:
    SelectableObject() : mIndex(-1) {};
    virtual ~SelectableObject(){};

    /**
     * Function that needs to be called just to configure the entity with this
     * object (entity->setUserAny(this), and set the associated flags)
     */
    void configureEntity(Ogre::MovableObject *ent,
                         unsigned int queryFlags,
                         unsigned int visibilityFlags =
                             Ogre::MovableObject::getDefaultVisibilityFlags());

    /**
     * Set / get the type of the object. This is really ugly but we can avoid
     * the reinterpret_cast this way and have a cleaner design
     */
    inline void setType(Type t);
    inline Type type(void) const;

    /**
     * @brief Returns the aditional info of this object
     * @returns The additional info associated to this object
     */
    inline const Info &additionalInfo(void) const;
    inline Info &additionalInfo(void);
    inline void setAdditionalInfo(Info &af);

    /**
     * This function have to be implemented that reproduce the effects when
     * the object is selected
     */
    virtual void objectSelected(void) = 0;

    /**
     * Function called when the object is not anymore selected
     */
    virtual void objectUnselected(void) = 0;

    /**
     * Function called when the mouse is over the object / unit
     */
    virtual void mouseOverObject(void) = 0;

    /**
     * Function called when the mouse is exit of the unit
     */
    virtual void mouseExitObject(void) = 0;

protected:
    /**
     * Add an object to the SelectionManager. This will automatically call
     * the functions objectSelected / objectUnselected
     */
    void selectThisOne(void);
    void unselectThisOne(void);

protected:
    Type mType;

private:
    // Let Selection manager to be friend of this class to improve the
    // search using the index right here
    friend class SelectionManager;
    int mIndex;
    Info mAdditionalInfo;
};



inline void
SelectableObject::setType(Type t)
{
    mType = t;
}
inline Type
SelectableObject::type(void) const
{
    return mType;
}
inline const Info &
SelectableObject::additionalInfo(void) const
{
    return mAdditionalInfo;
}
inline Info &
SelectableObject::additionalInfo(void)
{
    return mAdditionalInfo;
}
inline void
SelectableObject::setAdditionalInfo(Info &af)
{
    mAdditionalInfo = af;
}


}

#endif /* SELECTABLEOBJECT_H_ */