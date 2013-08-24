/*
 * GraphColoringHelper.h
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#ifndef GRAPHCOLORINGHELPER_H_
#define GRAPHCOLORINGHELPER_H_


#include <vector>

#include <types/basics.h>

namespace core {

namespace GraphColoringHelper {

// Define the adjacency matrix here
//
class AdjacencyMatrix {
public:
    AdjacencyMatrix() : mNumNodes(0) {}
    ~AdjacencyMatrix(){};

    // @brief set/get the number of nodes
    //
    inline void
    setNumberNodes(core::size_t N)
    {
        mNumNodes = N;
        mMatrix.resize(N*N, false);
    }
    inline core::size_t
    getNumNodes(void) const
    {
        return mNumNodes;
    }

    // @brief Set/get the value of a sepecific position in the matrix
    //
    inline void
    set(core::size_t i, core::size_t j, bool value)
    {
        mMatrix[getIndex(i,j)] = value;
        // mirror also
        mMatrix[getIndex(j,i)] = value;
    }
    inline bool
    get(core::size_t i, core::size_t j) const
    {
        return mMatrix[getIndex(i,j)];
    }

private:
    // @brief Returns the position in the matrix from a given row and column
    //
    inline core::size_t
    getIndex(core::size_t i, core::size_t j) const
    {
        return mNumNodes * i + j;
    }
private:
    std::vector<bool> mMatrix;
    core::size_t mNumNodes;
};

// @brief This class will color a graph given by a matrix (adjacency matrix).
// @param matrix    The adjacency matrix
// @param result    The list of the colors for each node (i-th position in the
//                  vector represent the color for the i-th node).
// @return the number of colors used
//
int
graphColoring(const AdjacencyMatrix& matrix, std::vector<int>& result);


}

} /* namespace core */
#endif /* GRAPHCOLORINGHELPER_H_ */
