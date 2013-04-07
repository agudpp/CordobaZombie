/*
 * PlayerUnit.cpp
 *
 *  Created on: 02/03/2012
 *      Author: agustin
 */

#include "PlayerUnit.h"
#include "PlayerDefs.h"
#include "CollectableObjTypes.h"

#include <SelectionSystem/SelectionType.h>

PlayerSMTTable *PlayerUnit::mSMTT = 0;
billboard::BillboardManager &PlayerUnit::mBillboardMngr =
    billboard::BillboardManager::instance();

const float  PlayerUnit::UPDATE_PATH_TIME = 0.5f;


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::attachWeaponToBone(Weapon *newWeapon, Weapon *oldWeapn)
{
	ASSERT(newWeapon);

	if(oldWeapn){
		mEntity->detachObjectFromBone(oldWeapn->getEntity());
	}
	mEntity->attachObjectToBone(PLAYER_HAND_BONE, newWeapon->getEntity());

}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::obtainAndAdviseNearbyZombies(void)
{
	// check all the objects that are close
	getNearbyObjects(ZOMBIE_MAX_VISIBILITY_DIST, ZOMBIE_MAX_VISIBILITY_DIST,
			COL_FLAG_UNIT_ZOMBIE, mGameObjectResult);

	// now for every object we put it into the nearbyZombies
	mNearbyZombies.clear();
	GameUnit *gu;
	for(int i = mGameObjectResult.size()-1; i >= 0; --i ){
		gu = static_cast<GameUnit *>(mGameObjectResult[i]);
		mNearbyZombies.push_back(gu);
		// advise the zu that we are closer
		gu->unitClose(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
PlayerUnit::PlayerUnit() :
		mFSM(this),
		mAttackingBehavior(PlayerUnit::BH_NORMAL),
		mWorldContext(this),
		mSelBillboard(0,0),
		mUpdatePathTime(0),
		mActualBomb(0),
		mLifeChangeCb(0),
		mPlayerID(PlayerID::NONE)
{
	// by default we use the mSMTT
	ASSERT(mSMTT);	// we must set this first
	mFSM.setTransitionTable(mSMTT);

	for(int i = 0; i < Weapon::W_NONE; ++i) mWeapons[i] = 0;
	mActualWeapon = 0;

	mIAState.event = IAEP_NONE;

	// set the type of this type of selectable object
	setType(selection::Type::SEL_TYPE_PLAYER);

}


////////////////////////////////////////////////////////////////////////////////
PlayerUnit::~PlayerUnit()
{
	// TODO: Que hacemos con las bombas?
	// TODO: que hacemos con los items?
	// TODO: que hacemos con las armas?
	// TODO: backpack?
}


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::objectSelected(void)
{
	if(mSelBillboard){
		// actually selected, change the atlas
		mBillboardMngr.changeAtlas(mSelBillboard, PLAYER_BB_SELECTION);
		return;
	}
	// get new billboard
	mSelBillboard = mBillboardMngr.getNewBillboard(PLAYER_BB_SELECTION);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::objectUnselected(void)
{
	if(mSelBillboard){
		mBillboardMngr.letAvailable(mSelBillboard);
		mSelBillboard.reset();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::mouseOverObject(void)
{
	// if is actually selected, we do nothing?
	if (mSelBillboard) {
	    return;
	}
	// else we get new billboard and use it
	mSelBillboard = mBillboardMngr.getNewBillboard(PLAYER_BB_MOUSE_OVER);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::mouseExitObject(void)
{
	// if we are already selected then we do nothing
	if(!mSelBillboard){
		return;
	}

	if(mBillboardMngr.getAtlas(mSelBillboard) == PLAYER_BB_SELECTION){
		// do nothing
		return;
	} else {
		// we are in "mouse over".. so we clear this billboard
		mBillboardMngr.letAvailable(mSelBillboard);
		mSelBillboard.reset();
	}
}


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::beenHit(const Hit_t &hit)
{
	// TODO:
	mLastHit = hit;
	// decrease the life
	setLife(getLife() - hit.power);

	// Advise calling the callback
	if(mLifeChangeCb) (*mLifeChangeCb)(0);

	// emmit a new event
	mFSM.newEvent(E_BEEN_HIT);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::borning(void)
{
	// TODO:
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::dead(void)
{
	// TODO:
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayerUnit::build(void)
{

	// load the animations of the entity
	std::vector<Ogre::String> anims;

	anims.push_back(PLAYER_ANIM_IDLE);
	anims.push_back(PLAYER_ANIM_WALK);
	anims.push_back(PLAYER_ANIM_RUN);
	anims.push_back(PLAYER_ANIM_ATTACK_B);
	anims.push_back(PLAYER_ANIM_ATTACK_S);
	anims.push_back(PLAYER_ANIM_ATTACK_S2);
	anims.push_back(PLAYER_ANIM_RELOAD_S);
	anims.push_back(PLAYER_ANIM_ATTACK_L);
	anims.push_back(PLAYER_ANIM_ATTACK_L2);
	anims.push_back(PLAYER_ANIM_RELOAD_L);
	anims.push_back(PLAYER_ANIM_ATTACK_T);
	anims.push_back(PLAYER_ANIM_PICK_OBJECT);
	anims.push_back(PLAYER_ANIM_PLANT_BOMB);
	anims.push_back(PLAYER_ANIM_GAB_MOV_OBJ);
	anims.push_back(PLAYER_ANIM_PUSH_MOV_OBJ);
	anims.push_back(PLAYER_ANIM_PULL_MOV_OBJ);
	anims.push_back(PLAYER_ANIM_DIE_L);
	anims.push_back(PLAYER_ANIM_DIE_R);
	anims.push_back(PLAYER_ANIM_DIE_B);
	anims.push_back(PLAYER_ANIM_DIE_F);
	anims.push_back(PLAYER_ANIM_HIT_F);
	anims.push_back(PLAYER_ANIM_HIT_B);

	loadAnimations(anims);

	// Attach unit's SceneNode to the SoundAPI
	ASSERT(getSceneNode() != 0);
	configureSoundAPI();

	// config the bounding box
	float w,h;
	getAABBFromEntity(w,h);
	configCollObj(w,h, COL_FLAG_UNIT_PLAYER, COL_GRFLAG_UNIT_PLAYER);

	// configure the unit path
	configureUnitPath();

	// Start the FSM
	mFSM.start();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::unitClose(GameUnit *unit)
{
	// TODO:
	ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::newExternalEvent(SMEvent e)
{
	if(mFSM.getLastEvent() != PlayerUnit::E_MOVE_TO){
		mFSM.newEvent(PlayerUnit::E_MOVE_TO);
		return;
	}

	mFSM.newEvent(e);
	// TODO: hacer bien aca todo
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::update(void)
{
	// TODO:

	// advise all the zombies closer to this unit
	obtainAndAdviseNearbyZombies();

	// update the world context
	mWorldContext.update(); // TODO: vamos a usar esto? o ni bosta?

	// update the actual animation
	mActualAnim->addTime(GLOBAL_TIME_FRAME * mAnimAccelCoef);
	mFSM.update();

	// if we have billboard, update the position
	if(mSelBillboard){
		mSelBillboard->setPosition(getPosition().x, getPosYAxis(), getPosition().y);
		// TODO: probablemente queremos rotarlo?
	}

}

///////						Function of this class					//////

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::addNewWeapon(Weapon *w)
{
	ASSERT(w);
	if(hasWeaponType(w->getType())){
		debugERROR("Tenemos que verificar si el arma ya existe entonces las "
				"fusionamos, en caso contrario solo la guardamos en la mochila.\n");
		// TODO: aca deberiamos fusionar armas?
		ASSERT(false);
		return;
	}

	// else we set that weapon

	// if the actual weapon is none, then we set this as actual
	if(getActualWeapon() == 0){
		mWeapons[w->getType()] = w;
		w->setOwner(this);
		setWeapon(w->getType());
	} else {
		// only add
		mWeapons[w->getType()] = w;
	}

	// create the representation
	createBackpackInterface(w);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::removeWeapon(int wtype)
{
	ASSERT(wtype < Weapon::W_NONE);
	ASSERT(mWeapons[wtype]);
	Weapon *w = mWeapons[wtype];

	// if is the actual weapon, then we remove and set another one?
	if(getActualWeapon() == w){
		// TODO: que hacemos aca? eliminamos el arma? la tiramos al piso?
		ASSERT(false);
		mWeapons[wtype] = 0;

		// Seteamos la primera que encontremos
		for(int i = 0; i <  Weapon::W_NONE; ++i) {
			if(getWeapon(i)){
				setWeapon(i);
				break;
			}
		}
	} else {
		// we onlye remove it
		// TODO: que hacemos aca? eliminamos el arma? la tiramos al piso?
		ASSERT(false);
		mWeapons[wtype] = 0;
	}
	mBackPack.removeBackpackItemUserDef(w);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::setWeapon(int wtype)
{
	ASSERT(wtype < Weapon::W_NONE);
	ASSERT(mWeapons[wtype]);
	attachWeaponToBone(mWeapons[wtype], getActualWeapon());
	mActualWeapon = wtype;
	// perform some animation? change weapon? ... NONE

}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::changeBestNextWeapon(void)
{
	// get the best weapon
	for(int i = 0; i < Weapon::W_NONE; ++i){
		if(mWeapons[i] && (mWeapons[i]->getAmmunition() > 0 ||
				mWeapons[i]->getAmmunition() < 0) &&
				mWeapons[i] != getActualWeapon()){
			// change to this weapon
			setWeapon(i);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::addBomb(Bomb *b)
{
	ASSERT(b);
	ASSERT(!hasBomb(b));
	debugERROR("Tenemos que verificar si la bomba ya existe entonces las "
					"fusionamos (o capaz que no se pueda), en caso contrario solo "
					"la guardamos en la mochila.\n");
	mBombs.push_back(b);
	createBackpackInterface(b);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::removeBomb(Bomb *b)
{
	ASSERT(b);
	ASSERT(hasBomb(b));
	mActualBomb = 0;
	mBackPack.removeBackpackItemUserDef(b);

	// remove the here
	int j = mBombs.size()-1;
	for(; j >= 0;--j) if(mBombs[j] == b) break;
	ASSERT(j >= 0);
	mBombs[j] = mBombs[mBombs.size()-1];
	mBombs.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayerUnit::plantBomb(Bomb *b, const sm::Vector2 &position)
{
	ASSERT(b);
	ASSERT(hasBomb(b));

	// we have to check that we have that object in the backpack
	debugBLUE("TODO: verificar que existe este objeto en la mochila?\n");

	// check if is a valid position to that.
	if(!getPathTo(position)){
		// no possible position
		return false;
	}

	// else we have a valid position, then we just start the logic
	mActualBomb = b;
	mFSM.newEvent(E_PLANT_BOMB);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::addItem(GameItem *i)
{
	ASSERT(!hasItem(i)); // TODO: no podemos alzar mas de 1 item igual??
#ifdef DEBUG
	BackpackItem *bi = mBackPack.hasBackpackItemUserDef(i);
	ASSERT(!bi);
#endif
	createBackpackInterface(i);
	mItems.push_back(i);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::removeItem(GameItem *i)
{
	ASSERT(hasItem(i));
	mBackPack.removeBackpackItemUserDef(i);
	// remove the item here
	int j = mItems.size()-1;
	for(; j >= 0;--j) if(mItems[j] == i) break;
	ASSERT(j >= 0);
	mItems[j] = mItems[mItems.size()-1];
	mItems.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::useItem(GameItem *i)
{
	ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::getNearbyPlayers(PlayerVector &players)
{
	// check all the objects that are close
	getNearbyObjects(PLAYER_MAX_VISIBILITY_DIST, PLAYER_MAX_VISIBILITY_DIST,
			COL_FLAG_UNIT_PLAYER, mGameObjectResult);

	// now for every object we put it into the nearbyZombies
	players.clear();
	PlayerUnit *pu;
	for(int i = mGameObjectResult.size()-1; i >= 0; --i ){
		pu = static_cast<PlayerUnit *>(mGameObjectResult[i]);
		players.push_back(pu);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::moveUnitTo(const sm::Vector2 &p)
{
	if(!getPathTo(p)){
		// TODO: reproducimos algun sonido de que no podemos movernos hasta ese
		// lugar?
		debug("EmittSound: We cannot move to that position!\n");
		return;
	}

	// we have a path.. set the unit movement
	// TODO: emitir sonido "OK, moviendonos..."

	// if the last event was Move_to then we dont change the state
	if(mFSM.getLastEvent() != PlayerUnit::E_MOVE_TO){
		mFSM.newEvent(PlayerUnit::E_MOVE_TO);
	}
}


////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::setSMTransitionTable(PlayerSMTTable *tt)
{
	ASSERT(tt);
	mSMTT = tt;
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::collectObject(CollectableObject *co){
	ASSERT(co);
	mTargetColObject = co;
	const Ogre::Vector3 & pos3 = co->getNode()->getPosition();
	sm::Vector2 pos2(pos3.x , pos3.z);
#ifdef DEBUG
	debugRED("Yendo a levantar el objeto en x = %f y = %f\n", pos2.x, pos2.y);
#endif
	if(!getPathTo(pos2)){
		// TODO: reproducimos algun sonido de que no podemos movernos hasta ese
		// lugar?
		debugRED("EmittSound: We cannot move to that position!\n");
		return;
	}

	mFSM.newEvent(PlayerUnit::E_PICK_OBJECT);
}

////////////////////////////////////////////////////////////////////////////////
void
PlayerUnit::addCollectObeject(CollectableObject *c)
{
	ASSERT(c);
	// check the type of the object and
	int cot = c->getType();
	switch(cot){
	case COT_BACKPACK_WEAPON:
	{
		// TODO: notar que deberiamos SI O SI poder aceptar este objeto
		// ASSERT(canPickObject(c));
		addNewWeapon(static_cast<Weapon *>(c->getObject()));
	}
	break;
	case COT_BACKPACK_BOMB:
	{
		addBomb(static_cast<Bomb *>(c->getObject()));
	}
	break;
	case COT_BACKPACK_ITEM:
	{
		addItem(static_cast<GameItem *>(c->getObject()));
	}
	break;

	}
}


////////////////////////////////////////////////////////////////////////////////
bool
PlayerUnit::canPickObject(void){
#ifdef DEBUG
	debugColor(DEBUG_YELLOW, "RELLENAR LA FUNCION CanPickObject\n");
#endif
	return true;
}
