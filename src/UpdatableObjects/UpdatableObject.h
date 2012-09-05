/*
 * UpdatableObject.h
 *
 *  Created on: 18/04/2012
 *      Author: agustin
 *
 */

#ifndef UPDATABLEOBJECT_H_
#define UPDATABLEOBJECT_H_

#include "UpdObjsManager.h"
#include "DebugUtil.h"

class UpdatableObject {
public:
	UpdatableObject(){};
	virtual ~UpdatableObject(){};

	/**
	 * Function called when we want to start to update the object
	 */
	inline void startUpdate(void);

	/**
	 * Function used to abort the update of this object
	 */
	inline void stopUpdate(void);

	/**
	 * Function called before the object start to bee updated
	 */
	virtual void beforeUpdate(void) = 0;

	/**
	 * Function to be implemented.
	 * This function is called every time the Manager is updated (every frame...)
	 */
	virtual void update(void) = 0;

	/**
	 * Function called when the object stop been updated
	 */
	virtual void updateStopped(void) = 0;

	/**
	 * Set the UpdatableObjectManager
	 */
	static void setUpdObjsManager(UpdObjsManager *mngr);

private:
	friend class UpdObjsManager;

	inline int getID(void);
	inline void setID(int i);

private:
	unsigned int			mID;
	static UpdObjsManager	*mMngr;
};



/**
 * Function called when we want to start to update the object
 */
inline void UpdatableObject::startUpdate(void)
{
	ASSERT(mMngr);
	if(mMngr->haveObject(this)) return;
	mMngr->addObject(this);
}

/**
 * Function used to abort the update of this object
 */
inline void UpdatableObject::stopUpdate(void)
{
	ASSERT(mMngr);
	mMngr->removeObject(this);
}

inline int UpdatableObject::getID(void)
{
	return mID;
}
inline void UpdatableObject::setID(int i)
{
	mID = i;
}


#endif /* UPDATABLEOBJECT_H_ */
