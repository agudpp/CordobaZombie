/*
 * TestingBullet.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "TestingBullet.h"

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include <memory>
#include <cstring>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>



// helper stuff
//
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);

    return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve(18); // just in case :p

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_A);
    buttons.push_back(input::KeyCode::KC_S);
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_W);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_DOWN);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_UP);
    buttons.push_back(input::KeyCode::KC_LSHIFT);
    buttons.push_back(input::KeyCode::KC_RSHIFT);
    buttons.push_back(input::KeyCode::KC_0);
    buttons.push_back(input::KeyCode::KC_1);
    buttons.push_back(input::KeyCode::KC_2);
    buttons.push_back(input::KeyCode::KC_E);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_ADD);
    buttons.push_back(input::KeyCode::KC_MINUS);
    buttons.push_back(input::KeyCode::KC_SPACE);

    return buttons;
}


// @brief Create a bullet object and put the rigid body into the world
//
tests::BulletObject*
createBulletObject(const Ogre::AxisAlignedBox& bb,
                   float mass,
                   btDiscreteDynamicsWorld* world)
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    core::Primitive* handPrim = pd.createBox(bb.getCenter(), bb.getSize(), pd.getFreshColour());
    tests::BulletObject* result = new tests::BulletObject;
    result->setNode(handPrim->node);

    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();

    const Ogre::Vector3 halfsize = bb.getHalfSize();
    const Ogre::Vector3 center = bb.getCenter();

    btCollisionShape* handShape = new btBoxShape(btVector3(halfsize.x, halfsize.y, halfsize.z));
    if (mass != 0.f) {
        handShape->calculateLocalInertia(mass,localInertia);
    }
    startTransform.setOrigin(btVector3(center.x,center.y,center.z));
    result->setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo handrbInfo(mass,result,handShape,localInertia);
    btRigidBody* handbody = new btRigidBody(handrbInfo);
    result->mRigidBody = handbody;
    world->addRigidBody(handbody);

    return result;
}

// @brief Calculate global position for a bone attached to a particular node
//
inline Ogre::Vector3
getWorldPosition(const Ogre::Bone* bone, const Ogre::SceneNode* node)
{
    ASSERT(bone);
    ASSERT(node);
    return node->getPosition() + node->getOrientation() * bone->_getDerivedPosition();
}

inline Ogre::Quaternion
globalOgreRotation(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
}
inline Ogre::Vector3
globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->getPosition() + parentNode->getOrientation() *
        bone->_getDerivedPosition();
}


// @brief draw a bone
//
inline void
drawBone(const Ogre::Bone *bone, const Ogre::SceneNode* node)
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    core::Primitive* prim = pd.create3DAxis(Ogre::Vector3::ZERO, 3);
    prim->node->setPosition(globalOgrePosition(bone, node));
    prim->node->setOrientation(globalOgreRotation(bone, node));
}


// @brief print the bone names of an entity
//
inline void
printBoneNames(Ogre::Entity* ent)
{
    if (ent == 0) {
        debugBLUE("Null entity\n");
        return;
    }
    Ogre::SkeletonInstance* skeleton = ent->getSkeleton();
    if (!skeleton) {
        debugBLUE("ent %s has no skeleton\n", ent->getName().c_str());
        return;
    }

    Ogre::Skeleton::BoneIterator boneIt = skeleton->getBoneIterator();
    unsigned int i = 0;
    while(boneIt.hasMoreElements()) {
        Ogre::Bone* bone = boneIt.getNext();
        debugBLUE("Bone[%d]: %s\n", i, bone->getName().c_str());
        ++i;
    }
}


}


namespace tests {

////////////////////////////////////////////////////////////////////////////////
Ogre::AxisAlignedBox
TestingBullet::bbFrom2Pos(const Ogre::Vector3& p1,
                          const Ogre::Vector3& p2,
                          float dim) const
{
    Ogre::Vector3 distVec = p1-p2;
    float len = distVec.length();
    Ogre::AxisAlignedBox bb;
    bb.setMinimum(0, -dim, -dim);
    bb.setMaximum(len, dim, dim);
    return bb;
}

core::Primitive*
TestingBullet::primitiveFromBones(const Ogre::Bone* b1,
                                  const Ogre::Bone* b2,
                                  const Ogre::SceneNode* node) const
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    Ogre::AxisAlignedBox bb = bbFrom2Pos(getWorldPosition(b1, node),
                                         getWorldPosition(b2, node),
                                         3);
    core::Primitive* armPrim = pd.createBox(Ogre::Vector3::ZERO,
                                            bb.getSize(),
                                            bb.getCenter(),
                                            pd.getFreshColour());
    // transform the box to be in the correct position
    Ogre::SceneNode* node2 = armPrim->node;
    node2->setPosition(getWorldPosition(b1, node));
    node2->setOrientation(node->_getDerivedOrientation() *
        b1->_getDerivedOrientation());

    return armPrim;
}


////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::loadFloor(void)
{
    Ogre::Plane p;
    p.normal = Ogre::Vector3(0, 0, 1);
    p.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);
    // Create an entity (the floor)
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("GrassSample");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::configureBullet(void)
{
    // set gravity
    mDynamicWorld.setGravity(btVector3(0,0,-50));

    // create scene
    btCollisionShape* groundShape =
        new btBoxShape(btVector3(btScalar(1520.),
                       btScalar(1550.),
                       btScalar(1.)));
    //keep track of the shapes, we release memory at exit.
    //make sure to re-use collision shapes among rigid bodies whenever possible!
    mCollisionShapes.push_back(groundShape);
    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, 0, 0));
    {
        btScalar mass(0.);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0,0,0);
        if (isDynamic)
            groundShape->calculateLocalInertia(mass,localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);

        //add the body to the dynamics world
        mDynamicWorld.addRigidBody(body);
    }

    return;

/*    {
        //create a dynamic rigidbody (a box in Ogre and bullet)
        core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
        core::Primitive* prim = pd.createBox(Ogre::Vector3::ZERO,
                                             Ogre::Vector3(15,15,15),
                                             Ogre::ColourValue::Green);

        mOgreBulletObject.setNode(prim->node);

        btCollisionShape* colShape = new btBoxShape(btVector3(15,15,15));
//        btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        mCollisionShapes.push_back(colShape);

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar    mass(1.f);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0,0,0);
        if (isDynamic)
            colShape->calculateLocalInertia(mass,localInertia);

            startTransform.setOrigin(btVector3(0,0,490));
            startTransform.setRotation(btQuaternion(btVector3(1,0,0), 1.14f));
            mOgreBulletObject.setWorldTransform(startTransform);

            //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,&mOgreBulletObject,colShape,localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            mOgreBulletObject.mRigidBody = body;

            mDynamicWorld.addRigidBody(body);
    }*/


    ////////////////////////////////////////////////////////////////////////////
    /// create Ragdoll...
    // create the primitives and assign it to the bulletobject
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    core::Primitive* handPrim = pd.createBox(Ogre::Vector3::ZERO,
                                         Ogre::Vector3(30,15,15),
                                         Ogre::ColourValue::Blue);
    core::Primitive* armPrim = pd.createBox(Ogre::Vector3::ZERO,
                                         Ogre::Vector3(30,15,15),
                                         Ogre::ColourValue::Green);
    core::Primitive* arm2Prim = pd.createBox(Ogre::Vector3::ZERO,
                                         Ogre::Vector3(30,15,15),
                                         Ogre::ColourValue::Red);
    mHand.setNode(handPrim->node);
    mArm.setNode(armPrim->node);
    mArm2.setNode(arm2Prim->node);

    btScalar    mass(1.f);
    btVector3 localInertia(0,0,0);
    btTransform startTransform;
    startTransform.setIdentity();

    btCollisionShape* handShape = new btBoxShape(btVector3(30,15,15));
    handShape->calculateLocalInertia(mass,localInertia);
    startTransform.setOrigin(btVector3(0,0,400));
    mHand.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo handrbInfo(mass,&mHand,handShape,localInertia);
    btRigidBody* handbody = new btRigidBody(handrbInfo);
    mHand.mRigidBody = handbody;

    startTransform.setIdentity();
    btCollisionShape* armShape = new btBoxShape(btVector3(30,15,15));
    armShape->calculateLocalInertia(mass,localInertia);
    startTransform.setOrigin(btVector3(40,0,400));
    mArm.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo armrbInfo(mass,&mArm,armShape,localInertia);
    btRigidBody* armbody = new btRigidBody(armrbInfo);
    mArm.mRigidBody = armbody;
    startTransform.setIdentity();

    btCollisionShape* arm2Shape = new btBoxShape(btVector3(30,15,15));
    arm2Shape->calculateLocalInertia(mass,localInertia);
    startTransform.setOrigin(btVector3(-40,0,400));
    mArm2.setWorldTransform(startTransform);
    btRigidBody::btRigidBodyConstructionInfo armrbInfo2(mass,&mArm2,arm2Shape,localInertia);
    btRigidBody* arm2body = new btRigidBody(armrbInfo2);
    mArm2.mRigidBody = arm2body;

    mDynamicWorld.addRigidBody(armbody);
    mDynamicWorld.addRigidBody(arm2body);
    mDynamicWorld.addRigidBody(handbody);

    // constraint
    mHand.mRigidBody->setDamping(0.05, 0.85);
    mHand.mRigidBody->setDeactivationTime(0.8);
    mHand.mRigidBody->setSleepingThresholds(1.6, 2.5);
    mArm.mRigidBody->setDamping(0.05, 0.85);
    mArm.mRigidBody->setDeactivationTime(0.8);
    mArm.mRigidBody->setSleepingThresholds(1.6, 2.5);
    mArm2.mRigidBody->setDamping(0.05, 0.85);
    mArm2.mRigidBody->setDeactivationTime(0.8);
    mArm2.mRigidBody->setSleepingThresholds(1.6, 2.5);


    btTransform localA, localB;
    localA.setIdentity(); localB.setIdentity();
    localA.getBasis().setEulerZYX(0,0,M_PI_2); localA.setOrigin(
        btVector3(btScalar(0.), btScalar(0), btScalar(0)));
    localB.getBasis().setEulerZYX(0,0,M_PI_2); localB.setOrigin(
        btVector3(btScalar(40), btScalar(0), btScalar(0)));
    mConstraint = new btConeTwistConstraint(*handbody, *armbody, localA, localB);
    mConstraint->setLimit(M_PI_4, M_PI_4, M_PI_2);

    localA.setIdentity(); localB.setIdentity();
    localA.setOrigin(btVector3(15,0,0));
    localB.setOrigin(btVector3(-15,0,0));
    btConeTwistConstraint* constraint2 = new btConeTwistConstraint(*handbody, *arm2body, localA, localB);
    constraint2->setLimit(M_PI_4, M_PI_4, M_PI_2);
    mDynamicWorld.addConstraint(constraint2, true);

//    mConstraint = new btPoint2PointConstraint(*handbody, *armbody,
//                                               btVector3(0,0,0),
//                                               btVector3(40,0,0));
    mDynamicWorld.addConstraint(mConstraint, true);

}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::createEntityAndShowBones(void)
{
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ent = mSceneMgr->createEntity("zombie.mesh");

    node->attachObject(ent);
    node->translate(100,150,125);
//    node->yaw(Ogre::Radian(2.f));
    mSkeletonNode = node;
    mModelNode = node;

    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();

    // iterate over all the bones
    Ogre::SkeletonInstance* skeleton = ent->getSkeleton();
    mSkeletonInstance = skeleton;
    mSkeleton = skeleton;
    ASSERT(skeleton);
    Ogre::Skeleton::BoneIterator boneIt = skeleton->getBoneIterator();
    while(boneIt.hasMoreElements()) {
        Ogre::Bone* bone = boneIt.getNext();
        ASSERT(bone);
        debugBLUE("Bone: %s\n", bone->getName().c_str());
        Ogre::Vector3 bp = node->getPosition() +
            node->getOrientation() * bone->_getDerivedPosition();
//        mBones.push_back(pd.createSphere(bp, 3, Ogre::ColourValue::Red));
    }
    Ogre::SkeletonInstance* s = skeleton;
    drawBone(s->getBone("B_HEAD"), node);
    drawBone(s->getBone("B_NECK"), node);
    drawBone(s->getBone("B_CLAVICLE_R"), node);
    drawBone(s->getBone("B_UPPER_ARM_R"), node);
    drawBone(s->getBone("B_FORE_ARM_R"), node);
    drawBone(s->getBone("B_HAND_R"), node);
    drawBone(s->getBone("B_THIGH_R"), node);
    drawBone(s->getBone("B_CALF_R"), node);
    drawBone(s->getBone("B_FOOT_R"), node);
    drawBone(s->getBone("B_CLAVICLE_L"), node);
    drawBone(s->getBone("B_UPPER_ARM_L"), node);
    drawBone(s->getBone("B_FORE_ARM_L"), node);
    drawBone(s->getBone("B_HAND_L"), node);
    drawBone(s->getBone("B_THIGH_L"), node);
    drawBone(s->getBone("B_CALF_L"), node);
    drawBone(s->getBone("B_FOOT_L"), node);
    drawBone(s->getBone(" B_SPINE"), node);
    drawBone(s->getBone("B_PELVIS"), node);

//    mAnimState = ent->getAnimationState("corre2");
//    mAnimState->setEnabled(true);
//    mAnimState->setLoop(true);

    // create 2 boxes for the "Bip01 R Forearm" to "Bip01 R Hand"
//    mArmBone = skeleton->getBone("Bip01 R Forearm");
//    Ogre::Bone* hand = skeleton->getBone("Bip01 R Hand");
//    core::Primitive* armPrim = primitiveFromBones(mArmBone, hand, node);
//    mBoxBoneInfo.push_back(BoxBoneInfo(armPrim,node,mArmBone));
//    core::Primitive* armPrim2 =
//        primitiveFromBones(skeleton->getBone("Bip01 L Forearm"),
//                           skeleton->getBone("Bip01 L Hand"),
//                           node);
//    mBoxBoneInfo.push_back(BoxBoneInfo(armPrim2,node,skeleton->getBone("Bip01 L Forearm")));
//    core::Primitive* armPrim3 =
//        primitiveFromBones(skeleton->getBone("Bip01 R Thigh"),
//                           skeleton->getBone("Bip01 R Calf"),
//                           node);
//    mBoxBoneInfo.push_back(BoxBoneInfo(armPrim3,node,skeleton->getBone("Bip01 R Thigh")));
//    core::Primitive* armPrim4 =
//        primitiveFromBones(skeleton->getBone("Bip01 L Thigh"),
//                           skeleton->getBone("Bip01 L Calf"),
//                           node);
//    mBoxBoneInfo.push_back(BoxBoneInfo(armPrim4,node,skeleton->getBone("Bip01 L Thigh")));
//
//    // we will manually controll all this bones
//    for (BoxBoneInfo& bi : mBoxBoneInfo) {
//        bi.bone->setManuallyControlled(true);
//    }
}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::createRagdoll(Ogre::SkeletonInstance* s, Ogre::SceneNode* sn)
{
    mRagdoll.setDynamicWorld(&mDynamicWorld);

    // old names
//    table[physics::BonesID::B_HEAD] = s->getBone("Bip01 Head");
//    table[physics::BonesID::B_NECK] = s->getBone("Bip01 Neck");
//    table[physics::BonesID::B_CLAVICLE_R] = s->getBone("Bip01 R Clavicle");
//    table[physics::BonesID::B_CLAVICLE_L] = s->getBone("Bip01 L Clavicle");
//    table[physics::BonesID::B_UPPER_ARM_R] = s->getBone("Bip01 R UpperArm");
//    table[physics::BonesID::B_UPPER_ARM_L] = s->getBone("Bip01 L UpperArm");
//    table[physics::BonesID::B_FORE_ARM_R] = s->getBone("Bip01 R Forearm");
//    table[physics::BonesID::B_FORE_ARM_L] = s->getBone("Bip01 L Forearm");
//    table[physics::BonesID::B_HAND_R] = s->getBone("Bip01 R Hand");
//    table[physics::BonesID::B_HAND_L] = s->getBone("Bip01 L Hand");
//    table[physics::BonesID::B_SPINE] = s->getBone("Bip01 Spine");
//    table[physics::BonesID::B_PELVIS] = s->getBone("Bip01 Pelvis");
//    table[physics::BonesID::B_THIGH_R] = s->getBone("Bip01 R Thigh");
//    table[physics::BonesID::B_THIGH_L] = s->getBone("Bip01 L Thigh");
//    table[physics::BonesID::B_CALF_R] = s->getBone("Bip01 R Calf");
//    table[physics::BonesID::B_CALF_L] = s->getBone("Bip01 L Calf");
//    table[physics::BonesID::B_FOOT_R] = s->getBone("Bip01 R Foot");
//    table[physics::BonesID::B_FOOT_L] = s->getBone("Bip01 L Foot");


    table.resize(table.max_size());
    table[physics::BonesID::B_HEAD] = s->getBone("B_HEAD");
    table[physics::BonesID::B_NECK] = s->getBone("B_NECK");
    table[physics::BonesID::B_CLAVICLE_R] = s->getBone("B_CLAVICLE_R");
    table[physics::BonesID::B_CLAVICLE_L] = s->getBone("B_CLAVICLE_L");
    table[physics::BonesID::B_UPPER_ARM_R] = s->getBone("B_UPPER_ARM_R");
    table[physics::BonesID::B_UPPER_ARM_L] = s->getBone("B_UPPER_ARM_L");
    table[physics::BonesID::B_FORE_ARM_R] = s->getBone("B_FORE_ARM_R");
    table[physics::BonesID::B_FORE_ARM_L] = s->getBone("B_FORE_ARM_L");
    table[physics::BonesID::B_HAND_R] = s->getBone("B_HAND_R");
    table[physics::BonesID::B_HAND_L] = s->getBone("B_HAND_L");
    table[physics::BonesID::B_SPINE] = s->getBone(" B_SPINE");
//    table[physics::BonesID::B_SPINE1] = s->getBone("Spine1");
    table[physics::BonesID::B_PELVIS] = s->getBone("B_PELVIS");
    table[physics::BonesID::B_THIGH_R] = s->getBone("B_THIGH_R");
    table[physics::BonesID::B_THIGH_L] = s->getBone("B_THIGH_L");
    table[physics::BonesID::B_CALF_R] = s->getBone("B_CALF_R");
    table[physics::BonesID::B_CALF_L] = s->getBone("B_CALF_L");
    table[physics::BonesID::B_FOOT_R] = s->getBone("B_FOOT_R");
    table[physics::BonesID::B_FOOT_L] = s->getBone("B_FOOT_L");
//    table[physics::BonesID::B_HEAD_NUB] = s->getBone("Bip01 HeadNub");
//    table[physics::BonesID::B_BIP01_ROOT] = s->getBone("Bip01");
//    table[physics::BonesID::B_TOE0_L] = s->getBone("Bip01 L Toe0");
//    table[physics::BonesID::B_TOE0_NUB_L] = s->getBone("Bip01 L Toe0Nub");
//    table[physics::BonesID::B_TOE0_R] = s->getBone("Bip01 R Toe0");
//    table[physics::BonesID::B_TOE0_NUB_R] = s->getBone("Bip01 R Toe0Nub");

    std::cout << " Building ragdoll: " << mRagdoll.buildFromSkeleton(table, sn) << std::endl;
    mHeadNode = mRagdoll.head;
}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::createSampleScene(void)
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    Ogre::Vector3 size(10,10,10);
    float currentZ = 10;
    for (unsigned int i = 0; i < 20; ++i) {
        for (unsigned int j = 0; j < 20; ++j) {
            Ogre::Vector3 min(j*size.x+2,0,currentZ);
            Ogre::Vector3 max((j+1)*size.x,10,currentZ + size.z);

            Ogre::AxisAlignedBox bb(min, max);

            createBulletObject(bb, 1, &mDynamicWorld);
        }
        currentZ += size.z + 2;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::handleRagdollInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_RSHIFT)) {
        if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
            mTranslationVec.y -= 1.0f;
        }
        if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
            mTranslationVec.y += 1.0f;
        }
    } else {
        if(mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
            mTranslationVec.z -= 1.0f;
        }
        if(mInputHelper.isKeyPressed(input::KeyCode::KC_DOWN)) {
            mTranslationVec.z += 1.0f;
        }
    }

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_RIGHT)) {
        mTranslationVec.x += 1.0f;
    }



    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mHeadNode->applyCentralImpulse(btVector3(mTranslationVec.x,
                                                 mTranslationVec.y,
                                                 mTranslationVec.z));
    }
}

////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mOrbitCamera.moveCamera(mTranslationVec);
    }
    if(zoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(zoom);
    }

    const float lMouseZ = input::Mouse::relZ();
    float scrollZoom = mOrbitCamera.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if(scrollZoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(input::Mouse::relX());
    const float lMouseY = float(input::Mouse::relY());
    if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
        const float factor = -0.01 * 1.5f;
        mOrbitCamera.rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));
    }

    // check for the type of camera we want to use
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_1)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
    }

}

TestingBullet::TestingBullet() :
    core::AppTester(mTimeFrame)
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
// Bullet config
,   mDispatcher(&mDefConf)
,   mDynamicWorld(&mDispatcher, &mBroadPhase, &mSeqSolver, &mDefConf)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

}

TestingBullet::~TestingBullet()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
TestingBullet::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    loadFloor();

    configureBullet();

//    createSampleScene();

//    createEntityAndShowBones();

    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ent = mSceneMgr->createEntity("zombie.mesh");
    mModelNode = node;
    node->attachObject(ent);
    node->translate(0,0,10);
    node->pitch(Ogre::Radian(M_PI_2), Ogre::Node::TS_WORLD);
    Ogre::SkeletonInstance* skeleton = ent->getSkeleton();
    createRagdoll(skeleton, node);
    printBoneNames(ent);

    // everything fine...


}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
TestingBullet::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // apply a force to the box up
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_SPACE)) {
        // apply a big impulse in the head
        mHeadNode->applyCentralImpulse(btVector3(0, -800,100));

//        mHeadNode->applyCentralImpulse(btVector3(0,0,100));
//        mHand.mRigidBody->applyCentralImpulse(btVector3(0,0,100));
//        mDynamicWorld.removeRigidBody(mHand.mRigidBody);
//        mDynamicWorld.removeRigidBody(mArm.mRigidBody);
//
//        btTransform t;
//        t.setIdentity();
//        t.setOrigin(btVector3(0,0,490));
//        mHand.mRigidBody->proceedToTransform(t);
//        t.setOrigin(btVector3(10,10,470));
//        mArm.mRigidBody->proceedToTransform(t);
//
//        mDynamicWorld.addRigidBody(mHand.mRigidBody);
//        mDynamicWorld.addRigidBody(mArm.mRigidBody);
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_0)) {
        mRagdoll.setEnable(false);
        mModelNode->setPosition(0,0,150);
        mRagdoll.configureRagdoll(table, mModelNode);
        mRagdoll.setEnable(true);
        mRagdollNeedToUpdate = true;
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_ADD)) {
        mModelNode->flipVisibility();
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_MINUS)) {
        mHeadNode->applyCentralImpulse(btVector3(0,-4000,1000));
    }

    // if click shoot a box
//    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left)) {
//        Ogre::Vector3 halfSize(6,6,6);
//        Ogre::Vector3 camPos = mOrbitCamera.getCameraPosition();
//
//        Ogre::AxisAlignedBox bb(camPos - halfSize, camPos + halfSize);
//        tests::BulletObject* box = createBulletObject(bb,
//                                                      3.f,
//                                                      &mDynamicWorld);
//
//        Ogre::Vector3 force(mCamera->getDerivedDirection());
//        force.normalise();
//        force *= 2600;
//        btVector3 btForce(force.x, force.y, force.z);
//        box->mRigidBody->applyCentralImpulse(btForce);
//    }


    // update the camera
    handleCameraInput();
    handleRagdollInput();
    mDynamicWorld.stepSimulation(mTimeFrame, 10);

    if (mRagdoll.isEnabled() && mRagdollNeedToUpdate) {
        mRagdollNeedToUpdate = mRagdoll.update();
    }

    // animate the player skeleton
//    Ogre::Skeleton::BoneIterator boneIt = mSkeleton->getBoneIterator();
//    core::Primitive** pri = mBones.begin();
//    while(boneIt.hasMoreElements()) {
//        Ogre::Bone* bone = boneIt.getNext();
//        ASSERT(bone);
//        Ogre::Vector3 bp = mSkeletonNode->getPosition() +
//            mSkeletonNode->getOrientation() * bone->_getDerivedPosition();
//        (*pri)->node->setPosition(bp);
//        ++pri;
//    }
//    for (unsigned int i = 0; i < mBoxBoneInfo.size(); ++i) {
//        mBoxBoneInfo[i].updatePrimitivePosition();
//    }
//    mAnimState->addTime(mTimeFrame);
}

}
