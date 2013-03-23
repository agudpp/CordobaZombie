/*
 * PlayerUnitBuilder.h
 *
 *  Created on: Mar 13, 2013
 *      Author: agustin
 */

#ifndef PLAYERUNITBUILDER_H_
#define PLAYERUNITBUILDER_H_

#include <OgreString.h>

#include <GameUnits/PlayerUnit/PlayerDefs.h>
#include <GameUnits/PlayerUnit/PlayerUnit.h>
#include <GameUnits/Weapon/WeaponBuilder.h>

class PlayerUnitBuilder
{
public:
    // @brief Static instance
    //
    static PlayerUnitBuilder &
    instance()
    {
        static PlayerUnitBuilder instance;
        return instance;
    }

    ~PlayerUnitBuilder();

    // @brief Set the XML file to use if we want to construct the players
    //        from an specific xml file. If we not set any xml file then we
    //        we will use the default values for the players.
    // @param xmlFile   The mm file to be used
    // @returns True if the file is ok
    //          false otherwise
    //
    bool
    useXmlFile(const Ogre::String &xmlFile);

    // @brief Construct a unit using the current configuration (default / xml)
    // @param playerId      The player ID to be built;
    // @returns PlayerUnit  A new (allocated) instance of a given player.
    //
    PlayerUnitPtr
    buildPlayer(PlayerID playerId) const;

    // @brief Returns the associated weapon builder just in case we need
    //        to build a weapon from other place
    // @returns weaponBuilder   The weapon builder used by this class
    inline WeaponBuilder &
    getWeaponBuilder(void);

private:
    // avoid copying
    PlayerUnitBuilder();
    PlayerUnitBuilder(const PlayerUnitBuilder&);
    PlayerUnitBuilder &operator=(const PlayerUnitBuilder&);

    // Specific players default constructions functions
    //
    void buildDefaultCardenal(PlayerUnitPtr &unit) const;
    void buildDefaultCoral(PlayerUnitPtr &unit) const;
    void buildDefaultTulian(PlayerUnitPtr &unit) const;

private:
    WeaponBuilder mWeaponBuilder;

};


// Inline impl
//

inline WeaponBuilder &
PlayerUnitBuilder::getWeaponBuilder(void)
{
    return mWeaponBuilder;
}


#endif /* PLAYERUNITBUILDER_H_ */
