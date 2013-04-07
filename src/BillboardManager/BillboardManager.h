/*
 * BillboardManager.h
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#ifndef BILLBOARDMANAGER_H_
#define BILLBOARDMANAGER_H_

#include <map>
#include <vector>
#include <list>

#include <boost/shared_ptr.hpp>

#include <OgreBillboard.h>
#include <OgreBillboardSet.h>

#include <Utils/GUI/AtlasOverlay/MultiAtlasOverlay.h>

#include "BillboardBatery.h"
#include "BillboardDefs.h"

namespace billboard {

class BillboardManager {

    typedef std::deque<Ogre::Billboard *>   BillboardDequeue;

    // Helper class used to handle the billboards as a Billboard Queue
    //
    class BillboardQueue {
    public:
        BillboardQueue();
        ~BillboardQueue();

        // build it
        void build(size_t count,                // num of billboards
                   const Ogre::String &matName, // material name
                   Ogre::Real width,
                   Ogre::Real height);

        // get a new billboard (or 0 if not possible)
        Ogre::Billboard * getNewBillboard(const UVCoords &coords);

        // returns a billboard to the queue
        void letAvailable(Ogre::Billboard *);

    private:

        Ogre::BillboardSet *mSet;
        Ogre::SceneNode *mNode;
        BillboardDequeue mBillboards;
    };
    typedef boost::shared_ptr<BillboardQueue> BillboardQueuePtr;

    // Helper container to handle the different atlas id's info
    //
    struct AtlasIDCoords {
        AtlasIDCoords(BillboardQueue *q, const UVCoords &c) :
            queue(q)
        ,   coords(c)
        {}

        BillboardQueue* queue;
        UVCoords coords;
    };

public:

    /**
     * @brief singleton instance
     */
    static BillboardManager &instance(void)
    {
        static BillboardManager instance;
        return instance;
    }

	~BillboardManager();


	////////////////////////////////////////////////////////////////////////////
	///                         Battery methods                              ///
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief Configure (create) a BillboardBattery.
	 * @param batInfo   The battery Info used to construct the battery
	 */
	inline void
	createBattery(const BatteryInfo &batInfo);

	/**
	 * @brief Get a Battery
	 * @param id    The BatteryID we want to get
	 * @returns the associated BillboardBattery
	 */
	inline BillboardBatery &
	getBattery(BatteryID id);


	////////////////////////////////////////////////////////////////////////////
	///                         General methods                              ///
	////////////////////////////////////////////////////////////////////////////

	/**
	 * TODO: Remove this function, use the future new system
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
	bool
	createSet(Ogre::Real r, const Ogre::String &mn, int size, int nAtla = 1);

	/**
	 * @brief Construct the map we want to use for this manager. The map is
	 *        and ID (int) -> uv coordinates. What we need to do also is create
	 *        a set of BillboardSet for each different size of billboards (all
	 *        billboards that have the same size will use the same BillboardSet).
	 * @param matName   Material name to be used
	 * @param atlasMap  The atlas map we will use
	 *
	 * @note  If the atlas was already constructed, then we will erase all the
	 *        old information
	 */
	void
	createAtlas(const Ogre::String &matName,
	            const std::map<unsigned int, AtlasInfo> &atlasMap);

	/**
	 * Returns if the manager is created or not
	 */
	bool
	isCreated(void);

	/**
	 * Set the billboards visible/invisible
	 */
	void
	setVisible(bool visible);

	/**
	 * Set the "bounds" where the billboards will be seen (normally the size of
	 * the level by now).
	 */
	void
	setBounds(const Ogre::AxisAlignedBox &bb, Ogre::Real radius);

	/**
	 * Get a new Billboard. If have no more billboard then 0 is returned
	 * @param	atlasNumber	If we are using atlas we can specify by parameter
	 * 						the atlas position we want to use (starting from 0)
	 */
	Ogre::Billboard *
	getNewBillboard(int atlasNumber = -1);

	/**
	 * Change the atlas of a billboard
	 * @param	bb			The billboard to modify
	 * @param	atlasNum	The atlas number to be set
	 */
	void
	changeAtlas(Ogre::Billboard *bb, int atlasNumber);

	/**
	 * Returns the atlas id of a billboard
	 */
	int
	getAtlas(Ogre::Billboard *bb);

	/**
	 * Let a new billboard available
	 */
	void
	letAvailable(Ogre::Billboard *);

	/**
	 * @brief Destroy all the billboards
	 */
	void
	destroyAll(void);

private:
    BillboardManager();
    BillboardManager(const BillboardManager&);
    BillboardManager& operator=(const BillboardManager&);

    /**
     * @brief Helper function that separate into different groups taking into
     *        account the size of the uv coords given a atlas map
     * @param atlasVec      The vector of pointers to AtlasInfo structs
     * @param result        The result vector of list containings the grouped
     *                      atlas info by uv width and height
     */
    void groupAtlasInfo(const std::vector<const AtlasInfo *> &atlasVec,
                        std::vector<std::list<const AtlasInfo *>> &result) const;

private:
    typedef std::map<unsigned int, AtlasIDCoords> AtlasIDHash;
    typedef std::pair<unsigned int, AtlasIDCoords> AtlasIDHashContent;
    typedef std::vector<BillboardQueuePtr> BillboardQueues;

	BillboardDequeue		mBillboards;
	Ogre::BillboardSet	*mBillboardSet;
	Ogre::SceneNode		*mNode;
	float				mAtlasSize;
	MultiAtlasOverlay mAtlasOverlay;
	BillboardBatery mBatteries[BatteryID::SIZE];
	AtlasIDHash mAtlasIDs;
	BillboardQueues mQueues;


};


// Inline implementations
//
inline void
BillboardManager::createBattery(const BatteryInfo &batInfo)
{
    ASSERT(batInfo.id < BatteryID::SIZE);
    ASSERT(!mBatteries[batInfo.id].isCreated());
    mBatteries[batInfo.id].createSet(batInfo.width, batInfo.height,
                                     batInfo.materialName,
                                     batInfo.count,
                                     batInfo.coords);
}


inline BillboardBatery &
BillboardManager::getBattery(BatteryID id)
{
    ASSERT(id < BatteryID::SIZE);
    return mBatteries[id];
}

}
#endif /* BILLBOARDMANAGER_H_ */
