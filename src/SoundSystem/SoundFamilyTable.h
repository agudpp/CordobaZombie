/*
 * SoundFamilyTable.h
 *
 *	Container of sounds specific to a unit (ZombieUnit, PlayerUnit, Weapon, ...)
 *
 *	Each GameUnit has its own SoundFamiliyTable, where the names of the files
 *	containing its sound effects are stored, following an enumerated code
 *	particular of the unit.
 *
 *	For instance, all ZombieUnits (normal or dog) have an available sound code
 *	(aka SSsoundCode) named SS_GRUNT_CODE. This code represents the position in
 *	their SoundFamilyTables where the file names of the zombie grunts (or zombie
 *	barks) are listed.
 *	Hence, thisZombieUnit->sAPI.play(mSounds.getSound(SS_GRUNT_CODE));
 *	will play a zombie grunt (or zombie bark)
 *
 *  Created on: Aug 7, 2012
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef SOUNDFAMILYTABLE_H_
#define SOUNDFAMILYTABLE_H_


#include <vector>
#include <cstdlib>  // RAND_MAX
#include <Ogre.h>
#include "DebugUtil.h"
#include "SoundEnums.h"



/*
 * TODO
 *
 * Hay una "categoría" de sonidos por cada class dentro de GameUnit
 * Las categorías son: ZOMBIES, CIVILIANS, PLAYERS, WEAPONS...
 *
 * Cada categoría tiene una interfaz unificada de acceso a los sonidos.
 * Por ejemplo los ZOMBIES: toda SSfamilyTable de esa categoría tiene:
 *	¤ randomGrunt()  // SS_GRUNT
 *	¤ mobGrunt()     // SS_MOB_GRUNT
 *	¤ hitSound()     // SS_HIT
 *	¤ dying()        // SS_DYING
 * Para PLAYERS:
 *  ¤ yesSir()       // SS_YES_SIR
 *  ¤ cantDoThat()   // SS_CANT_DO_THAT
 *  ¤ healing()      // SS_HEALING
 *  etc...
 *
 * La categoría se asocia a cada clase. Al momento de creación de la unidad
 * se especifica a cual familia de la categoría pertenece. Por ejemplo, cuando
 * se crea un player, se asocia a la tabla "mSounds" de su GameUnit la familia
 * de sonidos que corresponda (SS_PLAYER_CORAL_FAMILY, SS_PLAYER_TULIAN_FAMILY...)
 *
 * La reproducción de los sonidos está hardcodeada en la State Machine de cada
 * una de estas clases. Como la interfaz por categoría está unificada,
 * es posible llevar a cabo esta unificación de código.
 * Entonces cuando Coral haga sAPI.play(mSounds[SS_YES_SIR]) se escuchará
 * una voz de mujer, y cuando Tulián lo haga se escuchará una voz de hombre.
 *
 * Las tablas (familias de sonidos) estarán en archivos independientes incluidos
 * desde aquí. Se planea cargar estas tablas parseando archivos XML.
 */


typedef int SSsoundCode;

#define  MAX_FILENAME_LEN  (1<<7)  // 128 characters

/*
 * Generic family table (just a wrapper for an array of strings)
 */
class SoundFamilyTable
{
	typedef std::pair<Ogre::String*, uint> SoundsList;

public:
	inline SoundFamilyTable();
	inline virtual ~SoundFamilyTable();

	/**
	 ** @brief
	 ** Returns number of sound codes stored in this table.
	 **/
	inline uint
	getNumSounds() const;

	/**
	 ** @brief
	 ** Sets the array "sounds", of size "size", as the available file names
	 ** for the sound code "sc"
	 ** If sound code "sc" was not empty, "sounds" is appended at end.
	 **
	 ** @remarks
	 ** Array "sounds" must contain at least "size" elements.
	 ** Array "sounds" must contain file names, NOT full paths.
	 ** To improve memory usage, assign incremental sound codes:
	 ** v.gr. addSounds(0,...) ; addSounds(1,...) ; addSounds(2,...) ; etc.
	 **
	 ** @example
	 ** // Sets two filenames as choices for the sound code 'N'
	 ** const char sounds[2][30] = {"audio_1.ogg", "effect.wav"};
	 ** setSounds(N, sounds, 2);
	 **/
	void
	addSounds(SSsoundCode sc, const Ogre::String* sounds, uint size);

	/**
	 ** @brief
	 ** Returns file name of the specified sound code.
	 **
	 ** @remarks
	 ** Returns reference, not copy. XXX DO NOT MODIFY VALUE.
	 ** If the sound code had several file names associated,
	 ** the first one is returned (see getChosenRandomSound() below)
	 **
	 ** @returns
	 ** NULL      if sound code "sc" doesn't exist, or is empty.
	 ** reference to selected value otherwise.
	 **/
	inline const Ogre::String*
	getSound(SSsoundCode sc) const;

	/**
	 ** @brief
	 ** Returns a randomly chosen file name of the specified sound code.
	 **
	 ** @remarks
	 ** Returns reference, not copy. XXX DO NOT MODIFY VALUE.
	 **
	 ** @returns
	 ** NULL      if sound code "sc" doesn't exist, or is empty.
	 ** reference to selected value otherwise.
	 **/
	inline const Ogre::String*
	getRandomSound(SSsoundCode sc) const;

	/**
	 ** @brief
	 ** From a sound code with multiple posibilities,
	 ** returns a specific file name.
	 **
	 ** @remarks
	 ** Returns reference, not copy. XXX DO NOT MODIFY VALUE.
	 **
	 ** @returns
	 ** NULL      if sound code "sc" doesn't exist, or is empty.
	 ** NULL      if "choice" is not a viable choice.
	 ** reference to selected valude otherwise.
	 **/
	inline const Ogre::String*
	getChosenRandomSound(SSsoundCode sc, uint choice) const;

private:
	std::vector<SoundsList> mList;  // Sounds names lists
};



/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
SoundFamilyTable::SoundFamilyTable()
{
	/* Default constructor suffices. */
}


////////////////////////////////////////////////////////////////////////////////
inline
SoundFamilyTable::~SoundFamilyTable()
{
	for (uint i=0 ; i < mList.size() ; i++) {
		delete[] mList[i].first;
	}
	mList.clear();
}


////////////////////////////////////////////////////////////////////////////////
inline uint SoundFamilyTable::getNumSounds() const { return mList.size(); }


////////////////////////////////////////////////////////////////////////////////
inline const Ogre::String*
SoundFamilyTable::getSound(SSsoundCode sc) const
{
	if (mList.size() <= sc || !mList[sc].first || !mList[sc].second) {
		debugWARNING("Sound code #d empty or inexistent.\n", sc);
		return NULL;
	} else {
		return &mList[sc].first[0];
	}
}


////////////////////////////////////////////////////////////////////////////////
inline const Ogre::String*
SoundFamilyTable::getRandomSound(SSsoundCode sc) const
{
	if (mList.size() <= sc || !mList[sc].first || !mList[sc].second) {
		debugWARNING("Sound code #d empty or inexistent.\n", sc);
		return NULL;
	} else {
		int step = RAND_MAX / mList[sc].second;
		return &mList[sc].first[ std::rand() / step ];
	}
}


////////////////////////////////////////////////////////////////////////////////
const Ogre::String*
SoundFamilyTable::getChosenRandomSound(SSsoundCode sc, uint choice) const
{
	if (mList.size() <= sc || !mList[sc].first || mList[sc].second < choice) {
		return NULL;
	} else {
		return &mList[sc].first[choice];
	}
}





#endif /* SOUNDFAMILYTABLE_H_ */
