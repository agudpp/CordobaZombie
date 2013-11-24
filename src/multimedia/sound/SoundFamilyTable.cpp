/*
 * SoundFamilyTable.cpp
 *
 *  Created on: Aug 7, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <cstring>
#include <debug/DebugUtil.h>
#include "SoundFamilyTable.h"

namespace mm {

////////////////////////////////////////////////////////////////////////////////
void
SoundFamilyTable::addSounds(SSsoundCode sc, const Ogre::String* sounds, uint size)
{
	unsigned int discarded(0u);
	unsigned int oldSize = mList[sc].second;
	Ogre::String* newSounds(0);

	if (!sounds) {
		debugWARNING("NULL vector, nothing to do.\n");
		return;
	} else if(!size) {
		debugWARNING("Empty vector, nothing to do.\n");
		return;
	} else if (sc < 0) {
		debugWARNING("Negative sound codes (like %d) are not allowed.\n", sc);
		return;
	}

	if (mList.size() <= sc || !mList[sc].first) {
		// If this is the first call for this sound code, create list.
		newSounds = new Ogre::String[size];
	} else {
		// If sound code "sc" wasn't empty, append new items at end.
		uint oldSize = mList[sc].second;
		Ogre::String* newSounds = (Ogre::String*) realloc (mList[sc].first,
															oldSize + size);
	}
	ASSERT(newSounds);

	for (uint i=0,j=0 ; i < size ; i++) {
		// Avoid empty filenames
		if (sounds[i].empty())
			discarded++;
		else
			newSounds[oldSize+j++] = sounds[i];  // <3 this shit
	}
	if (discarded > 0)
		newSounds = (Ogre::String*) realloc (newSounds, oldSize+size-discarded);
	if (mList.size() <= sc)
		mList.resize(sc+1);
	mList[sc] = std::make_pair(newSounds, oldSize+size-discarded);  // m-leak?

	// Shrink vector, in case extra memory was reserved.
	if (mList.capacity() > mList.size()) {
		std::vector<SoundsList>(mList).swap(mList);
	}
}

}
