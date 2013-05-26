/*
 * PlayerUnitBuilder.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: agustin
 */

#include "PlayerUnitBuilder.h"

#include <DebugUtil/DebugUtil.h>
#include <GlobalObjects/GlobalObjects.h>
#include <Math/CommonMath.h>
#include <GameUnits/GameObject.h>

#include "StateMachine/PlayerFSMBuilder.h"


#define MAX_DEFAULT_VELOCITY_PLAYERS 180.0f



////////////////////////////////////////////////////////////////////////////////
void
PlayerUnitBuilder::buildDefaultCardenal(PlayerUnitPtr &unit) const
{
    ASSERT(unit.get());
    ASSERT(unit->playerID() == PlayerID::PLAYER_CARDENAL);

    debugWARNING("Using bad values here... probably we want to put anothers...\n");

    // Assume that collision manager, transtition table, and all the other
    // stuff were set to the players and all the other objects
    Ogre::Entity *entity = GLOBAL_SCN_MNGR->createEntity("cardenal.mesh");
    Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
    node->showBoundingBox(true);

    unit->setEntity(entity);
    unit->setSceneNode(node);
    unit->setHeight(5);
    unit->setLife(100);
    unit->setMaxVelocity(MAX_DEFAULT_VELOCITY_PLAYERS * 2.0f);
    unit->setVelocity(MAX_DEFAULT_VELOCITY_PLAYERS);
    unit->build();

    // set the 0,0 position
    math::Vector2 pos(100.f, 100.f);
    unit->setPosition(pos);


    // set a default weapon
    debugWARNING("We should use weapon builder here...\n");

//    Gun9mm *w = new Gun9mm;
//    w->setOwner(mPlayers.back());
//    w->setPower(1);
//    w->setSqrRange(90000.0);
//    w->setID(WP_ID_9MM);
//    w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
//    Weapon::Ammunition ammo;
//    ammo.ammo = 25;
//    w->setAmmunition(ammo);
//    mPlayers.back()->addNewWeapon(w);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnitBuilder::buildDefaultCoral(PlayerUnitPtr &unit) const
{
    ASSERT(unit.get());
    ASSERT(unit->playerID() == PlayerID::PLAYER_CORAL);

    debugWARNING("Using bad values here... probably we want to put anothers...\n");

    // Assume that collision manager, transtition table, and all the other
    // stuff were set to the players and all the other objects
    Ogre::Entity *entity = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
    Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
    node->showBoundingBox(true);

    unit->setEntity(entity);
    unit->setSceneNode(node);
    unit->setHeight(5);
    unit->setLife(100);
    unit->setMaxVelocity(MAX_DEFAULT_VELOCITY_PLAYERS * 2.0f);
    unit->setVelocity(MAX_DEFAULT_VELOCITY_PLAYERS);
    unit->build();

    // set the 0,0 position
    math::Vector2 pos(100.f, 100.f);
    unit->setPosition(pos);



    // set a default weapon
    debugWARNING("We should use weapon builder here...\n");

//    Gun9mm *w = new Gun9mm;
//    w->setOwner(mPlayers.back());
//    w->setPower(1);
//    w->setSqrRange(90000.0);
//    w->setID(WP_ID_9MM);
//    w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
//    Weapon::Ammunition ammo;
//    ammo.ammo = 25;
//    w->setAmmunition(ammo);
//    mPlayers.back()->addNewWeapon(w);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnitBuilder::buildDefaultTulian(PlayerUnitPtr &unit) const
{
    ASSERT(unit.get());
    ASSERT(unit->playerID() == PlayerID::PLAYER_TULIAN);

    debugWARNING("Using bad values here... probably we want to put anothers...\n");

    // Assume that collision manager, transtition table, and all the other
    // stuff were set to the players and all the other objects
    Ogre::Entity *entity = GLOBAL_SCN_MNGR->createEntity("tulian.mesh");
    Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
    node->showBoundingBox(true);

    unit->setEntity(entity);
    unit->setSceneNode(node);
    unit->setHeight(5);
    unit->setLife(100);
    unit->setMaxVelocity(MAX_DEFAULT_VELOCITY_PLAYERS * 2.0f);
    unit->setVelocity(MAX_DEFAULT_VELOCITY_PLAYERS);
    unit->build();

    // set the 0,0 position
    math::Vector2 pos(100.f, 100.f);
    unit->setPosition(pos);



    // set a default weapon
    debugWARNING("We should use weapon builder here...\n");

//    Gun9mm *w = new Gun9mm;
//    w->setOwner(mPlayers.back());
//    w->setPower(1);
//    w->setSqrRange(90000.0);
//    w->setID(WP_ID_9MM);
//    w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
//    Weapon::Ammunition ammo;
//    ammo.ammo = 25;
//    w->setAmmunition(ammo);
//    mPlayers.back()->addNewWeapon(w);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PlayerUnitBuilder::PlayerUnitBuilder()
{
}

PlayerUnitBuilder::~PlayerUnitBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayerUnitBuilder::useXmlFile(const Ogre::String &xmlFile)
{
    ASSERT(false && "Implement this");
    return false;
}

////////////////////////////////////////////////////////////////////////////////
PlayerUnitPtr
PlayerUnitBuilder::buildPlayer(PlayerID playerId) const
{
    // Build the player we need
    PlayerUnitPtr result(new PlayerUnit());
    result->setPlayerID(playerId);

    switch (playerId) {
    case PlayerID::PLAYER_TULIAN: buildDefaultTulian(result); break;
    case PlayerID::PLAYER_CORAL: buildDefaultCoral(result); break;
    case PlayerID::PLAYER_CARDENAL: buildDefaultCardenal(result); break;
    default:
        debugERROR("Error building a undefined player: %d\n", playerId);
        ASSERT(false);
    }

    return result;
}
