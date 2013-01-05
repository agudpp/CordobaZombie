/*
 * GameUnitHelper.h
 *
 *  Created on: 02/12/2012
 *      Author: agustin
 */

#ifndef GAMEUNITHELPER_H_
#define GAMEUNITHELPER_H_

class GameUnit;

namespace helper {

class GameUnitHelper {
public:
    /**
     * Full destroy a GameUnit (removing the scene node, entity, material, etc)
     * and deleting the object (free the memory).
     * @param   unit    The unit to be destroyed
     */
    static void destroyUnit(GameUnit *unit);
};

}

#endif /* GAMEUNITHELPER_H_ */
