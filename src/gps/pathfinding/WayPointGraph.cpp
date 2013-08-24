/*
 * WayPointGraph.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#include <fstream>

#include "WayPointGraph.h"


namespace {
struct Header {
    char info[4];
};
}

namespace gps {

////////////////////////////////////////////////////////////////////////////////
bool
WayPointGraph::containValidInfo(void) const
{
    if (mNodes.data == 0 || mNodes.size == 0) {
        debugWARNING("The graph is empty?\n");
        return false;
    }
    for (unsigned int i = 0; i < mNodes.size; ++i) {
        for (unsigned int j = 0, size = mNodes[i].neighborsCount; j < size; ++j) {
            if (mNodes[i].neighbors[j] >= mNodes.size) {
                debugERROR("Invalid information for node %d with neighbor %d, "
                    "index:%d and size: %d\n", i, j, mNodes[i].neighbors[j], mNodes.size);
                return false;
            }
        }
    }

    // everything fine
    return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
WayPointGraph::WayPointGraph()
{
}

////////////////////////////////////////////////////////////////////////////////
WayPointGraph::~WayPointGraph()
{
    // remove the memory if we need
    mNode.freeMemory();
}

////////////////////////////////////////////////////////////////////////////////
bool
WayPointGraph::build(const std::vector<WayPointNode>& nodes)
{
    mNode.freeMemory();

    // now we will allocate the needed memory for the container
    mNodes.data = malloc(nodes.size() * sizeof(WayPointNode));
    std::memcpy(mNodes.data, &(nodes[0]), nodes.size() * sizeof(WayPointNode));
    mNodes.size = nodes.size();

    // check that all the nodes contain valid indices
    //
    return containValidInfo();
}

////////////////////////////////////////////////////////////////////////////////
bool
WayPointGraph::build(const std::string& filename)
{
    mNode.freeMemory();

    // read the file
    std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!file) {
        debugERROR("Error opening the file %s\n", filename.c_str());
        return false;
    }

    // read the header first
    Header header;
    file.read(&header, sizeof(Header));
    if (std::strcmp(header.info, "WPG1") != 0) {
        debugERROR("Invalid graph version or header\n");
        return false;
    }

    // check the size and read all the information into the mem
    file.seekg(0, file.end);
    unsigned int length = file.tellg() - sizeof(Header);
    file.seekg(0, file.beg + sizeof(Header));
    mNodes.data = malloc(length);
    mNodes.size = length / sizeof(WayPointNode);
    file.read(mNodes.data, length);

    // check that is a valid information
    if (!containValidInfo()) {
        debugERROR("The file %s contains invalid information but correct header?\n",
            filename.c_str());
        mNode.freeMemory();
        return false;
    }

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
WayPointGraph::save(const std::string& filename) const
{
    if (mNode.data == 0 || mNode.size == 0) {
        debugWARNING("No graph information to save?\n");
        return false;
    }

    if (!containValidInfo()) {
        debugERROR("The graph contains invalid information, we cannot save this\n");
        return false;
    }

    std::ofstream file(filename.c_str(), std::ofstream::out | std::ofstream::binary);
    if (!file) {
        debugERROR("Error opening the file %s\n", filename.c_str());
        return false;
    }

    // save the header first
    Header h;
    std::strcpy(h.info, "WPG1");
    file.write(h.info, sizeof(Header));

    // save the nodes info
    file.write(mNodes.data, mNodes.size * sizeof(WayPointNode));
    file.close();

    return true;
}


} /* namespace gps */
