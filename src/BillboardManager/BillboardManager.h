/*
 * BillboardManager.h
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#ifndef BILLBOARDMANAGER_H_
#define BILLBOARDMANAGER_H_

#include <OgreBillboard.h>
#include <OgreBillboardSet.h>

#include "BillboardBatery.h"

class BillboardManager {
public:
	BillboardManager();
	~BillboardManager();

	/**
	 * Load a Billboardset using a radius create the billboards and
	 * a material
	 * @param r		The radius to be used
	 * @param mn	Material Name to be used
	 * @param size	The number of billboard to allocate
	 * @param nAtla If we are using atlas then we define the number of images
	 * 				that this material contains to be used as Atlas.
	 *
	 * @return 		True on success false otherwise
	 */
	bool createSet(Ogre::Real r, const Ogre::String &mn, int size, int nAtla = 1);

	/**
	 * Returns if the manager is created or not
	 */
	bool isCreated(void);

	/**
	 * Set the billboards visible/invisible
	 */
	void setVisible(bool visible);

	/**
	 * Set the "bounds" where the billboards will be seen (normally the size of
	 * the level by now).
	 */
	void setBounds(const Ogre::AxisAlignedBox &bb, Ogre::Real radius);

	/**
	 * Get a new Billboard. If have no more billboard then 0 is returned
	 * @param	atlasNumber	If we are using atlas we can specify by parameter
	 * 						the atlas position we want to use (starting from 0)
	 */
	Ogre::Billboard *getNewBillboard(int atlasNumber = -1);

	/**
	 * Change the atlas of a billboard
	 * @param	bb			The billboard to modify
	 * @param	atlasNum	The atlas number to be set
	 */
	void changeAtlas(Ogre::Billboard *bb, int atlasNumber);

	/**
	 * Returns the atlas id of a billboard
	 */
	int getAtlas(Ogre::Billboard *bb);

	/**
	 * Let a new billboard available
	 */
	void letAvailable(Ogre::Billboard *);


private:
	typedef std::deque<Ogre::Billboard *>	BillboardQueue;

	BillboardQueue		mBillboards;
	Ogre::BillboardSet	*mBillboardSet;
	Ogre::SceneNode		*mNode;
	float				mAtlasSize;

};

#endif /* BILLBOARDMANAGER_H_ */
