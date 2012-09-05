/*
 * GameObject.cpp
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#include "GameObject.h"

CollisionResult GameObject::mCollisionResult;
std::vector<GameObject *>	GameObject::mGameObjectResult;
CollisionManager *GameObject::collMng = 0;


////////////////////////////////////////////////////////////////////////////////
void GameObject::configCollObj(float w, float h, float mF, float mG)
{
	mCollObject.bb.setSize(w, h);
	mCollObject.maskFlag = mF;
	mCollObject.groupMask = mG;
	mCollObject.userDefined = this;

	ASSERT(collMng);
	collMng->addMovableObject(&mCollObject);
	mCollisionActive = true;

	// calculate the squared radius
	const float max = ((w > h) ? w : h) * 0.5f;
	mSqrRadius = max*max;

	ASSERT(mEntity);
	mEntity->setQueryFlags(mF);
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::getAABBFromEntity(float &w, float &h)
{
	ASSERT(mEntity);

	const Ogre::AxisAlignedBox &ab = mEntity->getBoundingBox();
	Ogre::Real r = ab.getHalfSize().length();
//	Ogre::Real r = mEntity->getBoundingRadius();
//	r *= 2.0f;
	w = r;
	h = r;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
GameObject::GameObject() :
		mEntity(0),
		mNode(0),
		mCollisionActive(false)
{
	// initialize the position of the object

}

////////////////////////////////////////////////////////////////////////////////
GameObject::~GameObject()
{
	// TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::setEntity(Ogre::Entity *ent)
{
	ASSERT(!mEntity);
	ASSERT(ent);
	mEntity = ent;
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::setSceneNode(Ogre::SceneNode *node)
{
	ASSERT(!mNode);
	ASSERT(node);
	mNode = node;
	ASSERT(mEntity);
	if(mEntity->getParentSceneNode() != node){
		// attach it
		mNode->attachObject(mEntity);
	}


//	// set the node to the object position
//	const sm::Vector2 &p = mCollObject.getPosition();
//	mNode->setPosition(p.x, getPosYAxis(), p.y);

}

////////////////////////////////////////////////////////////////////////////////
void GameObject::setActiveCollision(bool active)
{
	ASSERT(collMng);

	if(!active ){
		if(mCollisionActive){
			// remove the object from the place
			collMng->removeMovableObject(&mCollObject);
		}

		mCollisionActive = false;
	} else {
		// it already active?
		if(!mCollisionActive){
			// put it again
			collMng->addMovableObject(&mCollObject);
		}
		mCollisionActive = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::setPosition(const sm::Vector2 &p)
{
	sm::Vector2 trans = p - mCollObject.getPosition();
	translate(trans);

}
void GameObject::setPosition(const Ogre::Vector3 &p)
{
	sm::Vector2 trans = sm::Vector2(p.x, p.z) - mCollObject.getPosition();
	translate(trans);
}


////////////////////////////////////////////////////////////////////////////////
void GameObject::translate(const sm::Vector2 &t)
{
	ASSERT(mNode);

	// update the position of the collision Object
	if(mCollisionActive){
		ASSERT(collMng);
		collMng->translateObject(&mCollObject, t);
	} else {
		mCollObject.bb.translate(t);
	}

	// set the position of the scenenode
	const sm::Vector2 &p = mCollObject.getPosition();
	mNode->setPosition(p.x, getPosYAxis(), p.y);

}
void GameObject::translate(const Ogre::Vector3 &t)
{
	translate(sm::Vector2(t.x, t.z));
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::advance(const sm::Vector2 &m)
{
	ASSERT(mNode);

	// we will advance using the translate method of the scene node
	Ogre::Vector3 oldPos = mNode->getPosition();
	mNode->translate(m.x, getPosYAxis(), m.y);
	oldPos -= mNode->getPosition();


	// now oldPos get the translate vector... translate it
	if(mCollisionActive){
		ASSERT(collMng);
		collMng->translateObject(&mCollObject, sm::Vector2(oldPos.x, oldPos.z));
	} else {
		mCollObject.bb.translate(sm::Vector2(oldPos.x, oldPos.z));
	}

}

////////////////////////////////////////////////////////////////////////////////
void GameObject::advance(const Ogre::Vector3 &m)
{
	advance(sm::Vector2(m.x, m.z));
}


////////////////////////////////////////////////////////////////////////////////
void GameObject::lookAt(const Ogre::Vector3 &p)
{
	mNode->lookAt(Ogre::Vector3(p.x,getPosYAxis(),p.z), Ogre::SceneNode::TS_WORLD);
	updateActualDirection();
}


////////////////////////////////////////////////////////////////////////////////
const CollisionResult &GameObject::obtainCollisionObjects(void)
{
	mCollisionResult.clear();
	if(!mCollisionActive){
		debug("Warning: calling get collision without need\n");
		return mCollisionResult;
	}
	// TODO: implementar con flags
	ASSERT(collMng);
	collMng->getCollision(&mCollObject, mCollisionResult);

	return mCollisionResult;
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::getNearbyObjects(float mh, float mw, mask_t mask,
		std::vector<GameObject *> &result)
{
	ASSERT(mh > 0.0f);
	ASSERT(mw > 0.0f);

	static CollisionResult collresult;

	// create the AABB
	static sm::AABB aabb;
	aabb.tl.x = getPosition().x - mw;
	aabb.br.x = getPosition().x + mw;
	aabb.tl.y = getPosition().y + mh;
	aabb.br.y = getPosition().y - mh;

	collMng->getCollisionObjects(aabb, mask, collresult);

	result.clear();
	for(int i = collresult.size()-1; i >= 0; --i){
		ASSERT(collresult[i]);
		ASSERT(collresult[i]->userDefined);
		result.push_back(static_cast<GameObject *>(collresult[i]->userDefined));
	}
}
