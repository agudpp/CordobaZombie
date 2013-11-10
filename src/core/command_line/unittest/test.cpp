/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <algorithm>
#include <vector>
#include <cmath>
#include <set>
#include <string>
#include <UnitTest++/UnitTest++.h>

#include <command_line/CommandLine.h>


////////////////////////////////////////////////////////////////////////////////
// Definition of the CommandLine tools
//
//class CMDToolLoadSceneNode : public core::CommandLine {
//public:
//    int
//    execute(const std::vector<std::string>& args)
//    {
//        // load the scene node
//        int result = -1;
//        try {
//            Ogre::SceneNode* node = mSceneMgr->createSceneNode();
//        result = node != 0 ? 0 : -1;
//        mSceneMgr->destroySceneNode(node);
//        } catch (Ogre::Exception& e) {
//            debugERROR("Exception trying to create the node %s\n", e.what());
//        }
//        return result;
//    }
//};
class CMDToolLoadEntity : public core::CommandLine {
public:
    int
    execute(const std::vector<std::string>& args)
    {
        if (args.empty() || args[0].empty()) {
            return -1;
        }

        // load the scene node
        Ogre::SceneNode* node = mSceneMgr->createSceneNode();
        if (node == 0) return -1;

        // load the entity
        Ogre::Entity* ent = mSceneMgr->createEntity(args[0]);
        if (ent == 0) {
            return -1;
        }
        mSceneMgr->destroySceneNode(node);
        mSceneMgr->destroyEntity(ent);
        return 0;
    }
};

//
//TEST(CreateSimpleSceneNode)
//{
//    std::vector<std::string> args;
//    {
//        debugBLUE("Creating the tool instance\n");
//        CMDToolLoadSceneNode tool;
//        debugBLUE("Executing the tool instance\n");
//        CHECK_EQUAL(0,tool.execute(args));
//    }
//}

TEST(createAnEntityFromDropbox)
{
    std::vector<std::string> args;
    args.push_back("zombie.mesh");
    {
        debugBLUE("Creating the tool instance2\n");
        CMDToolLoadEntity tool;
        debugBLUE("Executing the tool instance2\n");
        CHECK_EQUAL(0, tool.execute(args));
    }

}


int
main(void)
{
    return UnitTest::RunAllTests();
}



