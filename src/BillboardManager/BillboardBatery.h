/*
 * BillboardBatery.h
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#ifndef BILLBOARDBATERY_H_
#define BILLBOARDBATERY_H_

#include <vector>

#include <OgreBillboard.h>
#include <OgreBillboardSet.h>

#include "BillboardDefs.h"

namespace billboard {

class BillboardBatery
{
public:
	BillboardBatery();
	~BillboardBatery();

	/**
	 * Load a Billboardset using a radius create the billboards and
	 * a material
	 * @param width  The width of the billboard we want to use
	 * @param height The height of the billboard we want to use
	 * @param mn	 Material Name to be used
	 * @param count	 The number of billboard to allocate
	 * @param coords The vector of coordinates to be used (uv coords). We will
	 *               use random uv coords in the billboards used
	 * @return 		True on success false otherwise
	 */
	bool
	createSet(Ogre::Real width, Ogre::Real height,
	          const Ogre::String &mn,
	          size_t count,
	          const std::vector<UVCoords> &coords);

	/**
	 * Hide all the billboards
	 */
	void
	setVisible(bool v);

	/**
	 * Show a new billboard in some position with some normal vector
	 */
	void
	showBillboard(const Ogre::Vector3 &pos);

	/**
	 * @brief Check if this battery is already created
	 */
	inline bool isCreated(void) const;


private:
	size_t mActualIndex;
	Ogre::BillboardSet *mBillboardSet;
	Ogre::SceneNode *mNode;
	std::vector<UVCoords> mCoords;
};


// Inline impl
//
inline bool
BillboardBatery::isCreated(void) const
{
    return mNode != 0;
}

}

#endif /* BILLBOARDBATERY_H_ */
