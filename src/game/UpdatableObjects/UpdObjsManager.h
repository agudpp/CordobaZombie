/*
 * UpdObjsManager.h
 *
 *  Created on: 18/04/2012
 *      Author: agustin
 *
 */

#ifndef UPDOBJSMANAGER_H_
#define UPDOBJSMANAGER_H_

#include <vector>



class UpdatableObject;

class UpdObjsManager {
public:
	/**
	 * Add a new objetc
	 */
	void addObject(UpdatableObject *obj);

	/**
	 * Remove an objetc
	 */
	void removeObject(UpdatableObject *obj);

	/**
	 * check if have an object
	 */
	bool haveObject(UpdatableObject *obj);

	/**
	 * Update all the objects
	 */
	void updateAllObjects(void);

private:
	typedef std::vector<UpdatableObject *>		ObjectsVec;

	ObjectsVec			mObjects;
};

#endif /* UPDOBJSMANAGER_H_ */
