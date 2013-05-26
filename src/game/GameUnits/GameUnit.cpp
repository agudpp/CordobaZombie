/*
 * GameUnit.cpp
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#include "GameUnit.h"


const float GameUnit::SQRD_PATHPOINT_DIST  =   30.0f;


////////////////////////////////////////////////////////////////////////////////
void GameUnit::loadAnimations(const std::vector<Ogre::String> &anims)
{
	ASSERT(anims.size());
	ASSERT(mEntity);

	delete []mAnimations; mAnimations = 0;

	mAnimations = new Ogre::AnimationState*[anims.size()];
	for(int i = 0; i < anims.size(); ++i){
		try{
			mAnimations[i] = mEntity->getAnimationState(anims[i]);
			mAnimations[i]->setEnabled(false);
			mAnimations[i]->setLoop(false);
		} catch(...){
			debug("The animation doesn't exists %s\n", anims[i].c_str());
			ASSERT(false);
		}
	}
	mActualAnimID = 0;
	mActualAnim = mAnimations[0];
}

////////////////////////////////////////////////////////////////////////////////
void GameUnit::configureUnitPath(void)
{
	mUnitPath.setRadius(std::sqrt(mSqrRadius));
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
GameUnit::GameUnit() :
		mAnimations(0),
		mActualAnim(0),
		mActualAnimID(0),
		mAnimAccelCoef(1.0f),
		mUnitGroup(0),
		mTarget(0),
		mLastTargetPos(0),
		mSAPI(0),
		mTimer(0)
{
	// TODO Auto-generated constructor stub

}


////////////////////////////////////////////////////////////////////////////////
GameUnit::~GameUnit()
{
	delete []mAnimations;
}

////////////////////////////////////////////////////////////////////////////////
int GameUnit::getLastHitImpactDir(void) const
{
	// get the normals of the left and right
	sm::Vector2 dir, ln, rn;
	getFieldOfVision(dir,ln,rn);

	// normalize all
	ln.normalize();
	rn.normalize();

	// we will create 4 vectors to divide the 360 posible directions on only 4
	// directions
	sm::Vector2 rdir = dir, v1,v2,v3;
	rdir.rotate180();
	v1 = ln + dir;
	v2 = ln + rdir;
	v3 = rn + rdir;
	// v4 is rdir now
	rdir = rn + dir;

	if(v1.isClockwise(mLastHit.hitDir)){
		if (!rdir.isClockwise(mLastHit.hitDir)){
			return Hit_t::FRONT;
		} else {
			return Hit_t::RIGHT;
		}
	} else {
		if(v2.isClockwise(mLastHit.hitDir)){
			return Hit_t::LEFT;
		}else {
			return Hit_t::BACK;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
bool GameUnit::getPathTo(const sm::Vector2 &p)
{
	mPathReady = false;
	const int r = PATHFINDER_INSTANCE.getPath(getPosition(),p,
			mUnitPath.getPointPath(), mUnitPath.getRadius());
	if (r == PathfindingManager::PATH_NOT_FOUND){
		return false;
	} else if(r == PathfindingManager::LINE_PATH){
		// we have a line path... set the only point
		mUnitPath.getPointPath().push_back(getPosition());
		mUnitPath.getPointPath().push_back(p);
	}
	mPathReady = true;	// by now we will get the path at once

	restartPath();
	return true;
}


////////////////////////////////////////////////////////////////////////////////
bool GameUnit::updateTargetPath(void)
{
	ASSERT(getActualTarget());

	const int r = GLOBAL_LVL_MNGR->getPathfinderManager()->
			updatePath(mUnitPath.getPointPath(), mUnitPath.getActualIndex(),
					getActualTarget()->getPosition(),
					mUnitPath.getRadius());
	if (r == PathfindingManager::PATH_NOT_FOUND){
		return false;
	} else if(r == PathfindingManager::LINE_PATH){
		// we have a line path... set the only the last point
		mUnitPath.getPointPath().back() = getActualTarget()->getPosition();

		// no need to update the path..
		return true;
	}
	mPathReady = true;	// by now we will get the path at once

	restartPath();
	return true;
}

////////////////////////////////////////////////////////////////////////////
////////////			STEERING BEHAVIORS				///////////////////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void GameUnit::repellingSteerVec(sm::Vector2 &result, const CollisionResult &objects)
{
	// initialize
	result.x = result.y = 0.0f;
	float sqrDist;
	sm::Vector2 repVec;
	sm::Vector2 dir, leftV, rightV;

	getFieldOfVision(dir, leftV, rightV);

	GameObject *obj;
	const CollisionObject *co;
	for(int i = objects.size()-1; i >= 0; --i){
		co = objects[i];
		ASSERT(co);

		obj = static_cast<GameObject *>(co->userDefined);

		// get the repelling vector
		repVec = co->getPosition();
		repVec -= getPosition();
//		repVec.transformToNormal();

		// check if the object is inside of the field of view
		// TODO: podemos ver de usar esto...
//		if(leftV.crossProduct(repVec) > 0.0f || //is counterclockwise
//				rightV.crossProduct(repVec) < 0.0f // is clockwise
//				)
//		{
//			// is not in the field of view
//			continue;
//		}

		// yes it is... change the direction of the vector
		repVec.rotate180();

		// get the distance of how much close is the other object to the
		// center of this one
		sqrDist = repVec.squaredLength();
		if(obj){
			sqrDist -= obj->getSqrRadius();
		} else {
			// TODO: aca deberiamos setear un tamaño mas real..
			sqrDist -= co->bb.getWidth()*0.5f;
		}
		if(sqrDist > getSqrRadius()){
			// the object is not colliding... do nothing
			continue;
		} else if (sqrDist < 0.0f){
			// the other object is inside of us... repell it at the maximum
			repVec.normalize();
			repVec *= getVelocity();
		} else {
			// we just have to apply a force proportional to the inside distance
			sqrDist = 1.0f - (sqrDist / getSqrRadius());
			if(sqrDist > 1.0f) sqrDist = 1.0f;
			ASSERT(sqrDist >= 0.0f);
			repVec.normalize();
			repVec *= sqrDist * getVelocity();
		}

		// calculate the result
		result += repVec*0.35;
	}

//	seekSteerVec(result, result +getPosition());
//	result.x = result.y = 0.0f; // TODO: eliminar esto
//	result.normalize();
//	result *= getVelocity();
}

////////////////////////////////////////////////////////////////////////////////
void GameUnit::seekSteerVec(sm::Vector2 &result, const sm::Vector2 &target)
{
	// go directly version...
	result = target;
	result -= getPosition();
	result.normalize();
	result *= getVelocity();
	return;


	// using desired velocity...
	// current velocity
	sm::Vector2 cv = getDirection();
	cv *= getVelocity();
	// desired velocity
	result = target;
	result -= getPosition();
	result.normalize();

	// result vel = desired velocity - current velocity
	result -= cv;
}

////////////////////////////////////////////////////////////////////////////////
bool GameUnit::followPathSteerVec(sm::Vector2 &result)
{
	// NEW IMPLEMENTATION
	sm::Vector2 dir;
	getDirection(dir);
	dir *= getVelocity();
	dir += getPosition();

	if(!mUnitPath.getNextPosition(getPosition(),dir,result)){
		result.x = result.y = 0.0f;
		return false;
	}
	dir = result;
	seekSteerVec(result, dir);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
void GameUnit::avoidanceSteerVec(sm::Vector2 &result, const GameObject *obj)
{
	ASSERT(obj);


	// we assume that the object is trying to avoid this object
	ASSERT(false);

	// TODO: vamos a hacer uno bien bien pedorro pero despues hay que cambiar
	// esto probablemente
	sm::Vector2 pl, pr, dir;

	dir = obj->getPosition();
	dir -= getPosition();

	pl = dir;
	pl.transformToNormal();
	pr = pl;
	pr.rotate180();

	// check the closest point and that will we use to follow

}

//-------------------	GROUP STEERING BEHAVIORS	-----------------------/

////////////////////////////////////////////////////////////////////////////
void GameUnit::cohesionSteerVec(sm::Vector2 &result)
{
	result.x = result.y = 0.0f;
	// if not in group return
	if(!isInGroup()){
		return;
	}

	// else we get the cohesion steering vector
	sm::Vector2 centerMass;
	UnitGroup::Container &c = getUnitGroup()->getAllUnits();
	int numNeighbor = 0;
	for(int i = c.size()-1; i >= 0; --i){
		// TODO: aca realmente chequeamos quienes son nuestros vecinos,
		// podriamos agrgar la funcion de si realmente lo estamos viendo
		// usando el "campo de vision" de la unidad.. y la distancia
		if(c[i] != this) {
			centerMass += c[i]->getPosition();
			++numNeighbor;
		}
	}

	// now calculate the center of mass position using an average
	if(numNeighbor){
		centerMass /= static_cast<float>(numNeighbor);
		seekSteerVec(result, centerMass);
	}
	// else we have not to do nothing
}

////////////////////////////////////////////////////////////////////////////
void GameUnit::alignmentSteerVec(sm::Vector2 &result)
{
	result.x = result.y = 0.0f;
	// if not in group return
	if(!isInGroup()){
		return;
	}

	// else we get the alignment average steering vector
	UnitGroup::Container &c = getUnitGroup()->getAllUnits();
	int numNeighbor = 0;
	for(int i = c.size()-1; i >= 0; --i){
		// TODO: aca realmente chequeamos quienes son nuestros vecinos,
		// podriamos agrgar la funcion de si realmente lo estamos viendo
		// usando el "campo de vision" de la unidad.. y la distancia
		if(c[i] != this) {
			result += c[i]->getDirection();
			++numNeighbor;
		}
	}

	// now calculate the center of mass position using an average
	if(numNeighbor){
		result /= static_cast<float>(numNeighbor);
	}
}

////////////////////////////////////////////////////////////////////////////
void GameUnit::separationSteerVec(sm::Vector2 &result)
{
	result.x = result.y = 0.0f;
	// if not in group return
	if(!isInGroup()){
		return;
	}

	// else we get the separation steering vector
	sm::Vector2 aux;
	UnitGroup::Container &c = getUnitGroup()->getAllUnits();
	for(int i = c.size()-1; i >= 0; --i){
		// TODO: aca realmente chequeamos quienes son nuestros vecinos,
		// podriamos agrgar la funcion de si realmente lo estamos viendo
		// usando el "campo de vision" de la unidad.. y la distancia
		if(c[i] != this) {
			aux = getPosition();
			aux -= c[i]->getPosition();
			aux.normalize();
			aux *= aux.inverseLen();
			result += aux;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
////////////		END	STEERING BEHAVIORS				///////////////////
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
bool GameUnit::haveLineOfSightTo(const GameUnit *other, float maxSqrDist) const
{
	ASSERT(other);

	float sqrDist = getPosition().squaredDistance(other->getPosition());
	if(sqrDist > maxSqrDist){
		return false;
	}

	// now we have to check if the vector formed is in the field of vision
	// of the unit!
	sm::Vector2 left, right, tmp;
	getFieldOfVision(tmp, left, right);
	tmp = getPosition() - other->getPosition();

	if(right.isClockwise(tmp) || !left.isClockwise(tmp)) {return false;}

	// else we are in the range of vision, perform a raycast
	const sm::Vector2 &p1 = getPosition(), &p2 = other->getPosition();
	collMng->getCollisionObjects(p1, p2, VISION_OBST_MASK,
			mCollisionResult);

	// if have no object return
	if(mCollisionResult.empty()){
		return true;
	}


	// check if some of this one is the unit and there are no other unit closer
	// than the unit... get all the intersections points
	static std::vector<sm::Point>	intersecPoints;
	float dist;
	for(int i = mCollisionResult.size()-1; i >= 0; --i){

// 		I commented the following lines because player and zombies
// are never vision obstacles ...

//		if(mCollisionResult[i]->userDefined == other ||
//				mCollisionResult[i]->userDefined == this){
//			continue;
//		}
		// else.. is other object, get the intersection points
		mCollisionResult[i]->getIntPoints(p1,p2,intersecPoints);
	//	ASSERT(!intersecPoints.empty());

		// get the closer point of all this ones
		for(int j = intersecPoints.size()-1; j >= 0; --j){
			dist = intersecPoints[j].squaredDistance(p1);
			if(dist < sqrDist){
				// there are other object first than the target
				return false;
			}
		}
	}

	// we can assume that the target is the closer object... and we can see it
	return true;
}


////////////////////////////////////////////////////////////////////////////////
	/**
	 * DESCRIPTION: Makes the game unit follow the path determined by the
	 * steering behaviors functions.
	 * RETURN VALUES: Returns true if he is going to move, false otherwise.
	 * NOTES:
	 */
bool GameUnit::moveThroughPath(void){
	// if we are not planting the bomb, we have to continue moving
	// Follow path logic
	sm::Vector2 mv, aux;

	if(!followPathSteerVec(aux)){
		return false;
	}
	mv += aux;

//	// we are moving... we are in a group?
//	if(ref->isInGroup()){
//		// calculate the steering vector of group moving
//		ref->separationSteerVec(aux);
//		move += aux;
//		ref->cohesionSteerVec(aux);
//		move += aux;
//		// Alignment we will not use this by now
//
//	}

	// check the collision steering vector
	if(!obtainCollisionObjects().empty()){
		repellingSteerVec(aux, getCollideObjects());
		mv += aux;
	}

	move(mv);

	return true;
}


