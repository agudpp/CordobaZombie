/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <Gimpact/btGImpactCollisionAlgorithm.h>
#include "GlobalObjects.h"
#include "Test.h"

class MyMotionState : public btMotionState {
public:
    MyMotionState(const btTransform &initialpos, Ogre::SceneNode *node) {
        mVisibleobj = node;
        mPos1 = initialpos;
    }

    virtual ~MyMotionState() {
    }

    void setNode(Ogre::SceneNode *node) {
        mVisibleobj = node;
    }

    virtual void getWorldTransform(btTransform &worldTrans) const {
        worldTrans = mPos1;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        if(NULL == mVisibleobj)
            return; // silently return before we set a node
        btQuaternion rot = worldTrans.getRotation();
        mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = worldTrans.getOrigin();
        mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
    }

protected:
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
};




// box
void CollObj::createObj(const Ogre::Vector3 &pos)
{
	ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
	node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(pos);

	Ogre::Vector3 size = ent->getBoundingBox().getHalfSize();

	btCollisionShape *shape = new btBoxShape(btVector3(size.x,size.y,size.z));
	btObj = new btPairCachingGhostObject();
	btObj->setCollisionShape(shape);
	btObj->setUserPointer(this);
	translateObject(pos);
}


void CollObj::translateObject(const Ogre::Vector3 &dir)
{
	lastPos = node->getPosition();
	Ogre::Vector3 newPos =  lastPos + dir;
	saveTransform = btObj->getWorldTransform();
	btTransform t(btQuaternion::getIdentity(), btVector3(newPos.x, newPos.y, newPos.z));
	btObj->setWorldTransform(t);
	node->setPosition(newPos);
}

void CollObj::setPosition(const Ogre::Vector3 &p)
{
	btTransform t(btQuaternion::getIdentity(), btVector3(p.x, p.y, p.z));
	btObj->setWorldTransform(t);
	node->setPosition(p);
}

void CollObj::restoreTransform(void)
{
	btObj->setWorldTransform(saveTransform);
	btVector3 &org = saveTransform.getOrigin();
	node->setPosition(org.x(),org.y(),org.z());
}

void CollObj::collision(const CollObj *other)
{
	ASSERT(other);
//	debug("Object %s collides with %s\n", name.c_str(), other->name.c_str());
//	setPosition(node->getPosition() + (lastPos - node->getPosition())*3.0f);
//	restoreTransform();
}






void Test::createCollisionWorld(void)
{
	btCollisionConfiguration *config = new btDefaultCollisionConfiguration();
	btCollisionDispatcher * dispatcher = new btCollisionDispatcher(config);
	btBroadphaseInterface *broadphase = new btDbvtBroadphase();

	mCollWorld = new btCollisionWorld(dispatcher, broadphase, config);

	// register the Collision Algorithm for GImpact Shapes
	dispatcher = static_cast<btCollisionDispatcher *>(mCollWorld->getDispatcher());
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

}

void Test::createBulletBox(const Ogre::Vector3 &pos, const Ogre::Vector3 &size)
{
	btCollisionShape* fallShape = new btBoxShape(btVector3(size.x*2.0f,size.y*2.0f,size.z*2.0f));
	//
	 debug("\n");
	 Ogre::Entity *e = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
	 Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	 node->attachObject(e);
	 node->setPosition(pos);

	 MyMotionState* fallMotionState =
					new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(pos.x,pos.y,pos.z)),node);
	 debug("\n");
	btScalar mass = 0;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass,fallInertia);
	 btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
	 fallRigidBodyCI.m_friction = 0.0f;
	 btRigidBody* frb = new btRigidBody(fallRigidBodyCI);
	//	fallRigidBody->setCollisionFlags(fallRigidBody->getCollisionFlags() |
	//			btCollisionObject::CF_KINEMATIC_OBJECT);
	dynamicsWorld->addRigidBody(frb);

}

void Test::oldTest(void)
{
	// Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));


	// Do_everything_else_here


	// Clean up behind ourselves like good little programmers
//	delete dynamicsWorld;
//	delete solver;
//	delete dispatcher;
//	delete collisionConfiguration;
//	delete broadphase;

	  btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
	  btCollisionShape* fallShape = new btBoxShape(btVector3(10,10,10));
	  debug("\n");
	  // plane
	  btDefaultMotionState* groundMotionState =
	                 new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	  btRigidBody::btRigidBodyConstructionInfo
	                 groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,10,0));
	         btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	 dynamicsWorld->addRigidBody(groundRigidBody);
	 debug("\n");

// sphere (using ogre State)
	 debug("\n");
	 ent = GLOBAL_SCN_MNGR->createEntity("ninja.mesh");
	 ballNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	 ballNode->attachObject(ent);
	 ballNode->setPosition(0,470,0);
	 MyMotionState* fallMotionState =
	                new MyMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,60,0)),ballNode);
	 debug("\n");
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass,fallInertia);
	 btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
	 fallRigidBodyCI.m_friction = 0.0f;
//	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody = new btPairCachingGhostObject();
	fallRigidBody->setCollisionShape(fallShape);



//	fallRigidBody->setCollisionFlags(fallRigidBody->getCollisionFlags() |
//			btCollisionObject::CF_KINEMATIC_OBJECT);
//	dynamicsWorld->addRigidBody(fallRigidBody);
	dynamicsWorld->addCollisionObject(fallRigidBody);
	btGhostPairCallback *cb = new btGhostPairCallback();
	dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(cb);
	dynamicsWorld->getPairCache()->setInternalGhostPairCallback(cb);


//	fallRigidBody->setLinearVelocity(btVector3(1,0,0));
	debug("\n");


	createBulletBox(Ogre::Vector3(100,10,0));
}

Test::Test()
{

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}



/* Load additional info */
void Test::loadAditionalData(void)
{
	createCollisionWorld();
	obj1 = new CollObj("Objeto1");
	obj2 = new CollObj("Objeto2");

	obj1->createObj(Ogre::Vector3(-100,10,0));
	obj2->createObj(Ogre::Vector3(100,10,0));

	mCollWorld->addCollisionObject(obj1->btObj);
	mCollWorld->addCollisionObject(obj2->btObj);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	mCollWorld->performDiscreteCollisionDetection();
	mCollWorld->updateAabbs();


	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_J)){
		obj1->translateObject(Ogre::Vector3(1,0,0));
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_L)){
		obj1->translateObject(Ogre::Vector3(-1,0,0));
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_I)){
		obj1->translateObject(Ogre::Vector3(0,0,1));
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_K)){
		obj1->translateObject(Ogre::Vector3(0,0,-1));
	}


	///one way to draw all the contact points is iterating over contact manifolds / points:
	const unsigned int  numManifolds = mCollWorld->getDispatcher()->getNumManifolds();
	for (unsigned int i=0;i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = mCollWorld->getDispatcher()->getManifoldByIndexInternal(i);

		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

		contactManifold->refreshContactPoints(obA->getWorldTransform(),obB->getWorldTransform());

		CollObj *collA = static_cast<CollObj*>(obA->getUserPointer());
		CollObj *collB = static_cast<CollObj*>(obB->getUserPointer());

		const unsigned int numContacts = contactManifold->getNumContacts();
		if(numContacts>0){
			collA->collision(collB);
			btManifoldPoint& pt = contactManifold->getContactPoint(0);
			Ogre::Vector3 p = collA->lastPos - collA->node->getPosition();
			p.normalise();
			p *= (pt.getDistance() + 1.0f);
			collA->setPosition(collA->node->getPosition() + p);
		}
		for (unsigned int j = 0;j < numContacts; j++)
		{
//			btManifoldPoint& pt = contactManifold->getContactPoint(j);
//				btVector3 ptA = pt.getPositionWorldOnA();
//				btVector3 ptB = pt.getPositionWorldOnB();
//				btVector3 ptDistB = ptB  + pt.m_normalWorldOnB *100;
//
//				mDebugContactPoints->addLine(ptA.x(),ptA.y(),ptA.z(),
//					ptB.x(),ptB.y(),ptB.z());
//				mDebugContactPoints->addLine(ptB.x(),ptB.y(),ptB.z(),
//					ptDistB.x(),ptDistB.y(),ptDistB.z());
//			}

		}
		//you can un-comment out this line, and then all points are removed
		//contactManifold->clearManifold();
	}










//	 dynamicsWorld->stepSimulation(GLOBAL_TIME_FRAME);
//	 dynamicsWorld->updateSingleAabb(fallRigidBody);
//	 if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
////		 fallRigidBody->setLinearVelocity();
////		 fallRigidBody->applyCentralForce(btVector3(0,0,0));
////		 fallRigidBody->translate(btVector3(0,0,10));
////		 fallRigidBody->setLinearVelocity(btVector3(0,0,0));
////		 fallRigidBody->;
//	 }
//	 if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
////		 fallRigidBody->setLinearVelocity(btVector3(100,0,0));
//		 static float x = -100;
//		 x+=1;
//		 btTransform tr(btQuaternion::getIdentity(), btVector3(x,10,0));
//		 fallRigidBody->setWorldTransform(tr);
//
//	 }
//	 if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_3)){
////		 fallRigidBody->setLinearVelocity(btVector3(-100,0,0));
//	 }
//
//
//	 btTransform &worldTrans = fallRigidBody->getWorldTransform();
//	 		 btQuaternion rot = worldTrans.getRotation();
//	 			ballNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
//	 			btVector3 pos = worldTrans.getOrigin();
//	 			ballNode->setPosition(pos.x(), pos.y(), pos.z());
//	 if(fallRigidBody->getNumOverlappingObjects() > 0){
//		 debug("Overlappung objects: %d\n",fallRigidBody->getNumOverlappingObjects());
//	 }
//
//	 // check collisions
//	 int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
//	for (int i=0;i<numManifolds;i++)
//	{
//		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
//		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
//		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
//		int numContacts = contactManifold->getNumContacts();
//		if(numContacts > 0){
////			debug("Contacts[%d, %d]\n", i, numContacts);
//			contactManifold->clearManifold();
//		}
////
////		for (int j=0;j<numContacts;j++)
////		{
////			btManifoldPoint& pt = contactManifold->getContactPoint(j);
////			if (pt.getDistance()<0.f)
////			{
////				const btVector3& ptA = pt.getPositionWorldOnA();
////				const btVector3& ptB = pt.getPositionWorldOnB();
////				const btVector3& normalOnB = pt.m_normalWorldOnB;
////			}
////		}
//	}
}
