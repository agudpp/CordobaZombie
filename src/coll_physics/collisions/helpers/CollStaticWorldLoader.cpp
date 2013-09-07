/*
 * CollStaticWorldLoader.cpp
 *
 *  Created on: Sep 6, 2013
 *      Author: agustin
 */

#include "CollStaticWorldLoader.h"

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreString.h>
#include <OgreResourceGroupManager.h>

#include <fstream>

#include <ogre_utils/DotSceneLoader.h>
#include <ogre_utils/OgreUtil.h>
#include <debug/DebugUtil.h>

namespace coll {

namespace CollStaticWorldLoader {


////////////////////////////////////////////////////////////////////////////
//                              Export methods                            //
////////////////////////////////////////////////////////////////////////////

bool
exportFromScene(const std::string& scene,
                Ogre::SceneManager* scnMngr,
                const std::string& output)
{
    ASSERT(scnMngr);

    struct NodeScopeDestroyer {
        Ogre::SceneNode* node;

        NodeScopeDestroyer(Ogre::SceneNode* n) :
            node(n)
        {}
        ~NodeScopeDestroyer()
        {
            ASSERT(node);
            node->getParentSceneNode()->removeAndDestroyChild(node->getName());
        }
    };

    // try to open the scene file and load it
    Ogre::DotSceneLoader dsl;
    Ogre::SceneNode* pnode = scnMngr->getRootSceneNode()->createChildSceneNode();
    NodeScopeDestroyer dpnode(pnode); // RAII

    debugWARNING("This is a DEBUG method, should be called only offline, not in "
        " the game itself! DO NOT REMOVE THIS COMMENT!\n");

    dsl.parseDotScene(scene,
                      Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                      scnMngr,
                      pnode);

    // get all the meshes from all the scene nodes in the root node (pnode)
    core::StackVector<Ogre::SceneNode*, 512> nodes;
    core::StackQueue<Ogre::SceneNode*, 512> nq;
    nq.push_back(pnode);

    while (!nq.empty()) {
        Ogre::SceneNode* current = nq.front();
        nq.pop_front();
        ASSERT(current);
        nodes.push_back(current);

        Ogre::Node::ChildNodeIterator it = current->getChildIterator();
        while(it.hasMoreElements()) {
            nq.push_back((Ogre::SceneNode*) it.getNext());
        }
    }

    // now extract the entities and its positions (node position)
    core::StackVector<std::pair<Ogre::Entity*, Ogre::Vector3>, 512> ents;
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        Ogre::SceneNode* current = nodes[i];
        Ogre::SceneNode::ObjectIterator it = current->getAttachedObjectIterator();
        while(it.hasMoreElements()) {
            ents.push_back(std::make_pair((Ogre::Entity*) it.getNext(),
                current->getPosition()));
        }
    }

    // for each mesh we will extract the vertices and the indices associated to them
    //
    LevelInfo::VerticesVec vertices;
    LevelInfo::ObjectIndicesVec indices;

    for (unsigned int i = 0; i < ents.size(); ++i) {
        unsigned int beginIndex = vertices.size();

        // read the vertices for this mesh
        Ogre::Vector3 verts[512];
        core::size_t vCount = 512;
        if (!core::OgreUtil::getContourVertices(ents[i].first->getMesh().get(),
                                                vCount,
                                                verts,
                                                ents[i].second)){
            debugERROR("Error getting contour vertices for mesh %s \n",
                ents[i].first->getName().c_str());
            // abort right now the process..
            return false;
        }
        unsigned int endIndex = beginIndex + vCount;

        // now we will push all this vertices into the vertices and the associated
        // indices
        for (unsigned int i = 0; i < vCount; ++i) {
            vertices.push_back(core::Vector2(verts[i].x, verts[i].y));
        }
        indices.push_back(std::make_pair(beginIndex, endIndex));
    }

    // now we will configure the bounding box of the level
    if (vertices.empty()) {
        debugWARNING("Empty scene file? %s, or some problem occur loading the"
            " meshes\n", scene.c_str());
        return false;
    }
    core::AABB staticBB(vertices[0], vertices[0]);
    for (core::Vector2* beg = vertices.begin(), *end = vertices.end();
            beg != end; ++beg) {
        staticBB.increaseToContain(*beg);
    }
    // start saving all the information
    std::ofstream file(output.c_str(), std::ofstream::binary);
    if (!file) {
        debugERROR("Error opening output file %s\n", output.c_str());
        return false;
    }

    // We will save the information in this order:
    // 1) BB
    // 2) All the indices (size, indices)
    // 3) All the vertices (size, vertices)
    //
    file.write(reinterpret_cast<const char*>(&staticBB), sizeof(core::AABB));
    unsigned int size = indices.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(indices.begin()),
               sizeof(std::pair<unsigned int, unsigned int>) * indices.size());
    size = vertices.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(vertices.begin()),
               sizeof(core::Vector2) * vertices.size());
    file.close();

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////
//                              Import methods                            //
////////////////////////////////////////////////////////////////////////////

bool
importFromFile(const std::string& filename, LevelInfo& levelInfo)
{
    // clear all the data
    levelInfo.indices.clear();
    levelInfo.vertices.clear();

    // open the file and read in the same order than it was saved
    // 1) BB
    // 2) All the indices (size, indices)
    // 3) All the vertices (size, vertices)

    std::ifstream file(filename.c_str(), std::ifstream::binary);

    if (!file) {
        debugERROR("Error opening the inpu file %s\n", filename.c_str());
        return false;
    }

    // read the data
    file.read(reinterpret_cast<char*>(&levelInfo.staticsBB), sizeof(core::AABB));

    // read the indices
    unsigned int size;
    file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
    if (size > levelInfo.indices.max_size()) {
        debugERROR("Invalid number of indices? %d\n", size);
        return false;
    }
    levelInfo.indices.resize(size);
    file.read(reinterpret_cast<char*>(levelInfo.indices.begin()),
              sizeof(std::pair<unsigned int, unsigned int>) * size);

    // read the vertices
    file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int));
    if (size > levelInfo.vertices.max_size()) {
        debugERROR("Invalid number of vertices? %d\n", size);
        return false;
    }
    levelInfo.vertices.resize(size);
    file.read(reinterpret_cast<char*>(levelInfo.vertices.begin()),
              sizeof(core::Vector2) * size);

    // everything ok!
    return true;
}


}
} /* namespace coll */
