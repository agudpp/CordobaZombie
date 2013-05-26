/*
 * SoundFamilyTable.cpp
 *
 *  Created on: Aug 7, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */


#include <cstring>
#include "DebugUtil.h"
#include "SoundFamilyTable.h"


////////////////////////////////////////////////////////////////////////////////
void
SoundFamilyTable::addSounds(SSsoundCode sc, const Ogre::String* sounds, uint size)
{
	ASSERT(!size || sounds);  // Preconditions
	if(!size) {
		debugWARNING("called with vector size == 0, nothing to do.\n");
		return;
	} else if (sc < 0) {
		debugERROR("Negative sound codes (like %d) are not allowed.\n", sc);
		return;
	}

	/* If this is the first call for this sound code, create list. */
	if (mList.size() <= sc || !mList[sc].first) {
		Ogre::String* newSounds(new Ogre::String[size]);
		/* Copy arguments. */
		for (uint i=0 ; i < size ; i++) {
			newSounds[i] = sounds[i];
		}
		mList.resize(sc+1);
		mList[sc] = std::make_pair(newSounds, size);

	/* If sound code "sc" wasn't empty, append new items at end. */
	} else {
		uint oldSize = mList[sc].second;
		Ogre::String* newSounds = (Ogre::String*) realloc (mList[sc].first,
														   oldSize + size);
		ASSERT(newSounds);
		for (uint i=0 ; i < size ; i++) {
			newSounds[oldSize+i] = sounds[i];
		}
		mList[sc] = std::make_pair(newSounds, oldSize + size);  // mem-leak?
	}

	/* Shrink vector, in case extra (useless) memory was reserved. */
	if (mList.capacity() > mList.size()) {
		std::vector<SoundsList>(mList).swap(mList);
	}
}

