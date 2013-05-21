/*
 * BillboardManager.cpp
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#include <algorithm>
#include <cmath>

#include "GlobalObjects.h"
#include "GUIHelper.h"

#include "BillboardManager.h"



// Helper methods
//
namespace {
/*
// Auxiliary function used to sort a vector of elements using different criterias
// given by the sort functors (in the order that are given)
//
inline template<typename _T, typename _Compare>
void
sortByCriterias(std::vector<_T> &vecToSort,
                const std::vector<_Compare> &compCriteria)
{
    ASSERT(!compCriteria.empty());

    // first sort with the first criteria
    std::sort(vecToSort.begin(), vecToSort.end(), compCriteria[0]);

    // now sort using all the other criterias
    typedef std::pair<size_t, size_t> VecPosPair;
    std::vector<VecPosPair> positions;
    positions.reserve(vecToSort.size() / 2); // just in case

    // sort for each criteria
    for (size_t i = 1, size = compCriteria.size(); i < size; ++i) {
        // get the indices
        positions.clear();
        size_t begin = 0;
        size_t end = 0;
        for (size_t j = 1, count = vecToSort.size(); j < count; ++j) {
            if (vecToSort[j-1])
        }
    }
}
*/
}


namespace billboard {

////////////////////////////////////////////////////////////////////////////////
BillboardManager::BillboardQueue::BillboardQueue() :
    mSet(0)
,   mNode(0)
{}

BillboardManager::BillboardQueue::~BillboardQueue()
{
    if(mSet->getParentSceneNode()){
        mSet->getParentSceneNode()->detachObject(mSet);
    }
    GLOBAL_SCN_MNGR->destroyBillboardSet(mSet);

    if(!mNode->getParentSceneNode()){
        GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
    }
    mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());
}

void
BillboardManager::BillboardQueue::build(size_t count,
                                        const Ogre::String &matName,
                                        Ogre::Real width,
                                        Ogre::Real height)
{
    ASSERT(!mSet);
    ASSERT(!mNode);

    mSet = GLOBAL_SCN_MNGR->createBillboardSet(count);

    // Generate the number of billboards we need
//  result->createBillboard(Ogre::Vector3::ZERO, Ogre::ColourValue::Green);
    mSet->setDefaultDimensions(width, height);
    mSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
    mSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
    mSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
    mSet->setMaterialName(matName);
    mSet->setVisible(true);

    // create the scene node
    mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

    // enqueue the billboards
    for(unsigned int i = 0; i < count; ++i){
        Ogre::Billboard *b = mSet->createBillboard(Ogre::Vector3(0,-9999.0f,0));
        ASSERT(b);
        mBillboards.push_back(b);
    }
}

// get a new billboard (or 0 if not possible)
Ogre::Billboard *
BillboardManager::BillboardQueue::getNewBillboard(const UVCoords &coords)
{
    ASSERT(mSet);
    if(mBillboards.empty()){
        return 0;
    }
    // check if we have to attach to the scene node
    if(mBillboards.size() == mSet->getPoolSize()){
        // this is the first that we will remove
        mNode->attachObject(mSet);
    }

    Ogre::Billboard *result = mBillboards.front();
    mBillboards.pop_front();

    // configure the atlas
    result->setTexcoordRect(coords.u0, coords.v0, coords.u1, coords.v1);

    return result;
}

// returns a billboard to the queue
void
BillboardManager::BillboardQueue::letAvailable(Ogre::Billboard *b)
{
    ASSERT(b);
    ASSERT(mBillboards.size()+1 <= mSet->getPoolSize());

    mBillboards.push_back(b);

    // check if it was the last one
    if(mBillboards.size() == mSet->getPoolSize()){
        mNode->detachObject(mSet);
    }

    // hide it
    b->setPosition(0,-9999.0f,0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::groupAtlasInfo(const std::vector<const AtlasInfo *> &av,
                                 std::vector<std::list<const AtlasInfo *> > &result) const
{
    result.clear();

    // make a copy of the vector
    std::vector<const AtlasInfo *> atlasVec = av;

    // sort the result using the width using a tolerance value (epsilon)
    // to avoid any kind of float rounding up stuff
    struct UVComparator {
        #define TOLERANCE 1.e-3f

        bool operator()(const AtlasInfo *a, const AtlasInfo *b)
        {
            if (useWidth) {
                const float abs = std::abs(a->width - b->width);
                return (abs > TOLERANCE) ? a->width < b->width : false;
            } else {
                // use height
                const float abs = std::abs(a->height - b->height);
                return (abs > TOLERANCE) ? a->height < b->height : false;
            }
        }
        bool useWidth;

        #undef TOLERANCE
    } uvComparator;

    uvComparator.useWidth = true;
    std::sort(atlasVec.begin(), atlasVec.end(), uvComparator);

    // resort each of this groups using the second criteria "height"
    uvComparator.useWidth = false;

    Ogre::Real compW = atlasVec[0]->width;
    size_t begin = 0;
    for (size_t i = 1, size = atlasVec.size(); i < size; ++i) {
        // push elements until we have a difference
        if (compW != atlasVec[i]->width) {
            // differ, sort this part!
            const size_t end = i - 1;
            if (begin != end) {
                // we need to sort
                std::sort(atlasVec.begin() + begin,
                          atlasVec.begin() + end,
                          uvComparator);
            }

            // update the indices
            begin = i;
            compW = atlasVec[begin]->width;
        }
    }


    // now we can assume that we have sort the atlasVector by the 2 criterias
    // we need (first width then height). We can now group them into different
    // groups depending of the size of the uvcoords

    typedef std::list<const AtlasInfo *> AtlasInfoList;

    result.push_back(AtlasInfoList());
    AtlasInfoList *currentList = &result.front();
    currentList->push_back(atlasVec.front());
    Ogre::Real lastWidth = currentList->back()->width;
    Ogre::Real lastHeight = currentList->back()->height;
    for (size_t i = 1, size = atlasVec.size(); i < size; ++i) {
        const Ogre::Real currentWidth = atlasVec[i]->width;
        const Ogre::Real currentHeight = atlasVec[i]->height;

        // check if we have to create a new
        if (lastWidth != currentWidth || lastHeight != currentHeight) {
            // we should create a new group
            result.push_back(AtlasInfoList());
            currentList = &result.back();
            lastWidth = currentWidth;
            lastHeight = currentHeight;
        }

        // add this one to the current list
        currentList->push_back(atlasVec[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BillboardManager::BillboardManager() :
mBillboardSet(0),
mNode(0),
mAtlasSize(-1)
{
	// TODO Auto-generated constructor stub

}

BillboardManager::~BillboardManager()
{
	if(mBillboardSet->getParentSceneNode()){
		mBillboardSet->getParentSceneNode()->detachObject(mBillboardSet);
	}
	GLOBAL_SCN_MNGR->destroyBillboardSet(mBillboardSet);

	if(!mNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
	}
	mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());
}

////////////////////////////////////////////////////////////////////////////////
bool
BillboardManager::createSet(Ogre::Real r,
                            const Ogre::String &mn,
                            int size,
                            int nAtlas)
{
    // TODO: @deprecated
	ASSERT(!mBillboardSet);

	mBillboardSet = GLOBAL_SCN_MNGR->createBillboardSet(size);

	// create the billboard
//	result->createBillboard(Ogre::Vector3::ZERO, Ogre::ColourValue::Green);
	mBillboardSet->setDefaultDimensions(r, r);
	mBillboardSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
	mBillboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	mBillboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	mBillboardSet->setMaterialName(mn);
	mBillboardSet->setVisible(true);

	// create the scene node
	mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	// enqueue the billboards
	for(int i = 0; i < size; ++i){
		Ogre::Billboard *b = mBillboardSet->createBillboard(Ogre::Vector3(0,-9999.0f,0));
		ASSERT(b);
		mBillboards.push_back(b);
	}

	// Configure the atlas size if we need it
    ASSERT(nAtlas > 0);
    if(nAtlas == 1){
        return true;
    }
    mAtlasSize = 1.0f / static_cast<float>(nAtlas);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::createAtlas(const Ogre::String &matName,
                              const std::map<unsigned int, AtlasInfo> &atlasMap)
{
    // first of all we will clear the "hash" or map
    destroyAll();

    if (atlasMap.empty()) {
        debugWARNING("We are receiving an empty atlasMap?\n");
        return;
    }

    // we need to extract all the different uv "container" sizes to create the
    // associated BillboardQueues.
    typedef std::map<unsigned int, AtlasInfo> MapType;
    std::vector<const AtlasInfo *> atlasInfoVec;

    atlasInfoVec.reserve(atlasMap.size());
    for (MapType::const_iterator it = atlasMap.begin(), eit = atlasMap.end();
        it != eit; ++it) {
        atlasInfoVec.push_back(&(it->second));

        // really ugly thing
        const_cast<AtlasInfo *>(&(it->second))->index = it->first;
    }

    std::vector<std::list<const AtlasInfo *> > groupedAtlas;
    groupAtlasInfo(atlasInfoVec, groupedAtlas);

    mQueues.reserve(groupedAtlas.size());

    // now we have grouped the atlas, we have to create groupedAtlas.size()
    // BillboardQueue's! One for each group
    for (size_t i = 0, size = groupedAtlas.size(); i < size; ++i) {
        ASSERT(groupedAtlas[i].size() >= 1);

        // we need to generate a BillboardQueue
        BillboardQueuePtr bqueuePtr(new BillboardQueue());
        size_t numBillboards = 0;

        mQueues.push_back(bqueuePtr);
        debugRED("groupedAtlas.size(): %u\tgroupedAtlas[i].size(): %u\n",
            groupedAtlas.size(), groupedAtlas[i].size());
        for (std::list<const AtlasInfo *>::const_iterator it = groupedAtlas[i].begin(),
                eit = groupedAtlas[i].end(); it != eit; ++it) {
            MapType::const_iterator mapIt = atlasMap.find((*it)->index);
            ASSERT(mapIt != atlasMap.end()); // should exists
            ASSERT(&(mapIt->second) == *it); // should be the same

            numBillboards += (*it)->count;

            // link in our map this id with the QueuePtr
            AtlasIDHash::iterator idIt = mAtlasIDs.find((*it)->index);
            debugBLUE("Iterating: %u\n", (*it)->index);
            ASSERT(idIt == mAtlasIDs.end()); // we have to ensure that we have
                                             // not two equal ids

            mAtlasIDs.insert(AtlasIDHashContent((*it)->index,
                AtlasIDCoords(bqueuePtr.get(), (*it)->uvs)));
        }
        // now we can build the BillboardQueue
        const AtlasInfo* ai = groupedAtlas[i].front();
        bqueuePtr->build(numBillboards, matName, ai->width, ai->height);
    }

}

////////////////////////////////////////////////////////////////////////////////
bool
BillboardManager::isCreated(void)
{
	return !mQueues.empty();
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::setVisible(bool visible)
{
    for (size_t i = 0, size = mQueues.size(); i < size; ++i) {
        mQueues[i]->set()->setVisible(visible);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::setBounds(const Ogre::AxisAlignedBox &bb, Ogre::Real radius)
{
    for (size_t i = 0, size = mQueues.size(); i < size; ++i) {
        mQueues[i]->set()->setBounds(bb, radius);
        mQueues[i]->set()->setCullIndividually(true);
    }
}

////////////////////////////////////////////////////////////////////////////////
BillboardWrapper
BillboardManager::getNewBillboard(unsigned int atlasNumber)
{
    AtlasIDHash::iterator it = mAtlasIDs.find(atlasNumber);
    if(it == mAtlasIDs.end()){
        // no atlas found
		return BillboardWrapper(0, 0);
	}

	// get the billboard from the associated BillboardQueue
    ASSERT(it->second.queue);
    BillboardQueue *queue = it->second.queue;
    return BillboardWrapper(queue->getNewBillboard(it->second.coords), atlasNumber);
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::changeAtlas(BillboardWrapper &bb, unsigned int atlasNumber)
{
	ASSERT(bb);

	AtlasIDHash::iterator it = mAtlasIDs.find(atlasNumber);
	if(it == mAtlasIDs.end()){
        // no atlas found
	    ASSERT(it != mAtlasIDs.end());
        return;
    }

	const UVCoords &coords = it->second.coords;
	bb->setTexcoordRect(coords.u0, coords.v0, coords.u1, coords.v1);
	bb.setAtlasID(atlasNumber);
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::letAvailable(BillboardWrapper &b)
{
    // TODO: change this to use the new version
	ASSERT(b.billboard());
	AtlasIDHash::iterator it = mAtlasIDs.find(b.atlasID());
    ASSERT(it != mAtlasIDs.end()); // we need to ensure this
    ASSERT(it->second.queue);

    it->second.queue->letAvailable(b);
}

////////////////////////////////////////////////////////////////////////////////
void
BillboardManager::destroyAll(void)
{
    mAtlasIDs.clear(); // this is slow, should be called on level start up
    mQueues.clear();

}


}
