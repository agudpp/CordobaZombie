/*
 * BillboardBatery.h
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#ifndef BILLBOARDBATERY_H_
#define BILLBOARDBATERY_H_


#include <OgreBillboard.h>
#include <OgreBillboardSet.h>


class BillboardBatery
{
public:
	BillboardBatery();
	~BillboardBatery();

	/**
	 * Load a Billboardset using a radius create the billboards and
	 * a material
	 * @param r		The radius to be used
	 * @param mn	Material Name to be used
	 * @param size	The number of billboard to allocate
	 * @param nrml	The normal vector to be used
	 *
	 * @return 		True on success false otherwise
	 */
	bool createSet(Ogre::Real r, const Ogre::String &mn, int size);

	/**
	 * Hide all the billboards
	 */
	void setVisible(bool v);

	/**
	 * Show a new billboard in some position with some normal vector
	 */
	void showBillboard(const Ogre::Vector3 &pos);


private:
	int					mActualIndex;
	Ogre::BillboardSet	*mBillboardSet;
	Ogre::SceneNode		*mNode;
};

#endif /* BILLBOARDBATERY_H_ */
